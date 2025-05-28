/*
 * Copyright (c) 2025 Valve Corporation.
 * Author: Changwoo Min <changwoo@igalia.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "energy_model.h" /* include/uapi/linux/energy_model.h */

static const struct nla_policy em_pd_genl_policy[EM_PD_GENL_ATTR_MAX + 1] = {
	[EM_PD_GENL_ATTR_ID]			= { .type = NLA_U32 },
	[EM_PD_GENL_ATTR_FLAGS]			= { .type = NLA_U64 },
	[EM_PD_GENL_ATTR_CPUS]			= { .type = NLA_STRING },
};

static const struct nla_policy em_tbl_genl_policy[EM_TBL_GENL_ATTR_MAX + 1] = {
	[EM_TBL_GENL_ATTR_PS_PERFORMANCE]	= { .type = NLA_U64 },
	[EM_TBL_GENL_ATTR_PS_FREQUENCY]		= { .type = NLA_U64 },
	[EM_TBL_GENL_ATTR_PS_POWER]		= { .type = NLA_U64 },
	[EM_TBL_GENL_ATTR_PS_COST]		= { .type = NLA_U64 },
	[EM_TBL_GENL_ATTR_PS_FLAGS]		= { .type = NLA_U64 },
};

struct nl_sock *sock;
int family_id, mc_id;

#define MAX_PD_IDS 4096
int pd_ids[MAX_PD_IDS];
int nr_pd_ids;

int init_nl_conn(void)
{
	int ret;

	sock = nl_socket_alloc();
	if (!sock) {
		fprintf(stderr, "Failed to allocate netlink socket\n");
		return 1;
	}

	if (genl_connect(sock)) {
		fprintf(stderr, "Failed to connect to generic netlink\n");
		return 1;
	}

	family_id = genl_ctrl_resolve(sock, EM_GENL_FAMILY_NAME);
	if (family_id < 0) {
		fprintf(stderr, "Failed to resolve energy_model family id\n");
		return 1;
	}

	mc_id = genl_ctrl_resolve_grp(sock, EM_GENL_FAMILY_NAME, EM_GENL_EVENT_GROUP_NAME);
	if (mc_id < 0) {
		fprintf(stderr, "Failed to resolve energy_model multicast group\n");
		return 1;
	}

	ret = nl_socket_add_membership(sock, mc_id);
	if (ret < 0) {
		fprintf(stderr, "Failed to join multicast group\n");
		return 1;
	}

	return 0;
}

static int cb_pd_get_id(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *attrs[EM_GENL_ATTR_MAX + 1];
	struct nlattr *pd_attrs[EM_PD_GENL_ATTR_MAX + 1];
	struct nlattr *cur;
	int rem;

	nl_msg_dump(msg, stdout);

	nla_parse(attrs, EM_GENL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!attrs[EM_GENL_ATTR_PD]) {
		fprintf(stderr, "Missing a mandatory attribute.\n");
		return NL_SKIP;
	}

	nla_for_each_nested(cur, attrs[EM_GENL_ATTR_PD], rem) {
		if (nla_parse_nested(pd_attrs, EM_PD_GENL_ATTR_MAX, cur, em_pd_genl_policy)) {
			fprintf(stderr, "Failed to parse performance domain information.\n");
			continue;
		}

		if (pd_attrs[EM_PD_GENL_ATTR_ID])
			printf("PD_ID: %d\n", nla_get_u32(pd_attrs[EM_PD_GENL_ATTR_ID]));
		if (pd_attrs[EM_PD_GENL_ATTR_FLAGS])
			printf("  PD_FLAGS: 0x%lx\n", nla_get_u64(pd_attrs[EM_PD_GENL_ATTR_FLAGS]));
		if (pd_attrs[EM_PD_GENL_ATTR_CPUS])
			printf("  PD_CPUS: %s\n", nla_get_string(pd_attrs[EM_PD_GENL_ATTR_CPUS]));

		pd_ids[nr_pd_ids++] = nla_get_u32(pd_attrs[EM_PD_GENL_ATTR_ID]);
		if (nr_pd_ids >= MAX_PD_IDS) {
			fprintf(stderr, "There are too many performance domains.\n");
			return NL_SKIP;
		}
	}

	return NL_OK;
}

int cmd_pd_get_id(void)
{
	struct nl_msg *msg;
	int ret;

	msg = nlmsg_alloc();
	if (!msg) {
		fprintf(stderr, "Failed to allocate netlink message\n");
		return 1;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0,
		    EM_GENL_CMD_PD_GET_ID, EM_GENL_VERSION);

	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, cb_pd_get_id, NULL);

	ret = nl_send_auto(sock, msg);
	if (ret < 0) {
		fprintf(stderr, "Failed to send netlink message\n");
		goto out_free;
	}

	ret = nl_wait_for_ack(sock);
	if (ret < 0) {
		fprintf(stderr, "Failed to wait for a ACK message\n");
		goto out_free;
	}

	ret = nl_recvmsgs_default(sock);
	if (ret < 0) {
		fprintf(stderr, "Failed to receive netlink responses: %d\n", ret);
		goto out_free;
	}

out_free:
	nlmsg_free(msg);
	return ret;
}

static int cb_pd_get_tbl(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *attrs[EM_GENL_ATTR_MAX + 1];
	struct nlattr *tbl_attrs[EM_TBL_GENL_ATTR_MAX + 1];
	struct nlattr *cur;
	int i, rem;

	nl_msg_dump(msg, stdout);

	nla_parse(attrs, EM_GENL_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!attrs[EM_GENL_ATTR_PD_TBL]) {
		fprintf(stderr, "Missing a mandatory attribute.\n");
		return NL_SKIP;
	}

	i = 0;
	nla_for_each_nested(cur, attrs[EM_GENL_ATTR_PD_TBL], rem) {
		if (nla_parse_nested(tbl_attrs, EM_TBL_GENL_ATTR_MAX, cur, em_tbl_genl_policy)) {
			fprintf(stderr, "Failed to parse performance domain information.\n");
			continue;
		}

		if (tbl_attrs[EM_TBL_GENL_ATTR_PS_PERFORMANCE])
			printf("[%d] PERFORMANCE: %lu\n", i, nla_get_u64(tbl_attrs[EM_TBL_GENL_ATTR_PS_PERFORMANCE]));
		if (tbl_attrs[EM_TBL_GENL_ATTR_PS_FREQUENCY])
			printf("  [%d] FREQUENCY: %lu\n", i, nla_get_u64(tbl_attrs[EM_TBL_GENL_ATTR_PS_FREQUENCY]));
		if (tbl_attrs[EM_TBL_GENL_ATTR_PS_POWER])
			printf("  [%d] POWER: %lu\n", i, nla_get_u64(tbl_attrs[EM_TBL_GENL_ATTR_PS_POWER]));
		if (tbl_attrs[EM_TBL_GENL_ATTR_PS_COST])
			printf("  [%d] COST: %lu\n", i, nla_get_u64(tbl_attrs[EM_TBL_GENL_ATTR_PS_COST]));
		if (tbl_attrs[EM_TBL_GENL_ATTR_PS_FLAGS])
			printf("  [%d] FLAGS: 0x%lx\n", i, nla_get_u64(tbl_attrs[EM_TBL_GENL_ATTR_PS_FLAGS]));

		i++;
	}

	return NL_OK;
}

int cmd_pd_get_tbl(int pd_id)
{
	struct nl_msg *msg;
	int ret;

	msg = nlmsg_alloc();
	if (!msg) {
		fprintf(stderr, "Failed to allocate netlink message\n");
		return 1;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0,
		    EM_GENL_CMD_PD_GET_TBL, EM_GENL_VERSION);

	nla_put_u32(msg, EM_PD_GENL_ATTR_ID, pd_id);

	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, cb_pd_get_tbl, NULL);

	ret = nl_send_auto(sock, msg);
	if (ret < 0) {
		fprintf(stderr, "Failed to send netlink message\n");
		goto out_free;
	}

	ret = nl_wait_for_ack(sock);
	if (ret < 0) {
		fprintf(stderr, "Failed to wait for a ACK message\n");
		goto out_free;
	}

	ret = nl_recvmsgs_default(sock);
	if (ret < 0) {
		fprintf(stderr, "Failed to receive netlink responses: %d\n", ret);
		goto out_free;
	}

out_free:
	nlmsg_free(msg);
	return ret;
}

static const char *event_to_str(int event)
{
	switch(event) {
	case EM_GENL_EVENT_PD_CREATE:
		return "EM_GENL_EVENT_PD_CREATE";
	case EM_GENL_EVENT_PD_DELETE:
		return "EM_GENL_EVENT_PD_DELETE";
	case EM_GENL_EVENT_PD_UPDATE:
		return "EM_GENL_EVENT_PD_UPDATE";
	};

	return "Unknown event";
}

static int cb_event(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct genlmsghdr *ghdr = genlmsg_hdr(nlh);
	struct nlattr *attrs[EM_PD_GENL_ATTR_MAX + 1];
	int ret;

	nl_msg_dump(msg, stdout);

	ret = genlmsg_parse(nlh, 0, attrs, EM_PD_GENL_ATTR_ID, NULL);
	if (ret < 0) {
		fprintf(stderr, "Failed to parse an incoming message\n");
		return NL_STOP;
	}

	printf("%s (%d)\n", event_to_str(ghdr->cmd), ghdr->cmd);
	if (attrs[EM_PD_GENL_ATTR_ID])
		printf("  PD_ID: %d\n", nla_get_u32(attrs[EM_PD_GENL_ATTR_ID]));

	return NL_OK;
}

static int cb_ignore_seq_check(struct nl_msg *msg, void *arg)
{
	/*
	 * Ignore the sequence number checking since we share a socket
	 * for requests and multicast events.
	 */
	return NL_OK;
}

int main(int argc, char *argv[])
{
	int ret;
	struct nl_cb *cb = NULL;

	/* Establish a netlink connection. */
	ret = init_nl_conn();
	if (ret) {
		fprintf(stderr, "Failed to fail to initialize the netlink connection.\n");
		goto out_err;
	}

	/* Request the list of information for all performance domains. */
	printf("==== List of performace domains.\n");
  	ret = cmd_pd_get_id();
	if (ret) {
		fprintf(stderr, "Failed to get pd id\n");
		goto out_err;
	}

	/* Request the energy model for each performance domain. */
	for (int i = 0; i < nr_pd_ids; i++) {
		int pd_id = pd_ids[i];

		printf("==== Energy model for performance domain %d\n", pd_id);
		ret = cmd_pd_get_tbl(pd_id);
		if (ret) {
			fprintf(stderr, "Failed to get pd id\n");
			goto out_err;
		}
	}

	/* Listening for kernel events. */
	printf("==== Literning for kernel events...\n");
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		fprintf(stderr, "Failed to allocate a callback.\n");
		goto out_err;
	}
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, cb_ignore_seq_check, NULL);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cb_event, NULL);

	while (1) {
		ret = nl_recvmsgs(sock, cb);
		if (ret < 0) {
			fprintf(stderr, "nl_recvmsgs_default() failed: %s\n", nl_geterror(ret));
			break;
		}
	}

	/* Tear down the netlink connection. */
	nl_cb_put(cb);
	nl_socket_free(sock);
	fprintf(stderr, "Finished with no error.\n");
	return 0;

	/* Exit with errors. */
out_err:
	if (cb)
		nl_cb_put(cb);
	if (sock)
		nl_socket_free(sock);
	fprintf(stderr, "Finished with some error.\n");
	return ret;
}

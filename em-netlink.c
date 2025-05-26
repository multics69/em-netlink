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

int main(int argc, char *argv[])
{
	int ret;

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

	/* Tear down the netlink connection. */
	nl_socket_free(sock);
	fprintf(stderr, "Finished with no error.\n");
	return 0;

	/* Exit with errors. */
out_err:
	if (sock)
		nl_socket_free(sock);
	fprintf(stderr, "Finished with some error.\n");
	return ret;
}

// SPDX-License-Identifier: Apache-2.0

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "btop_shared.hpp"

TEST(network, interface_list_preserves_order_and_removes_duplicates) {
	EXPECT_EQ(Net::parse_interface_list(""), std::vector<std::string> {});
	EXPECT_EQ(
		Net::parse_interface_list("enp1s0f0np0 enP2p1s0f0np0"),
		(std::vector<std::string> {"enp1s0f0np0", "enP2p1s0f0np0"})
	);
	EXPECT_EQ(
		Net::parse_interface_list("  enp1s0f0np0   enP2p1s0f0np0 enp1s0f0np0  "),
		(std::vector<std::string> {"enp1s0f0np0", "enP2p1s0f0np0"})
	);
}

TEST(network, physical_port_group_ignores_pcie_function) {
	const auto first = Net::physical_interface_id(
		"36982b000347bb4c", "p0", "0000:01:00.0", "enp1s0f0np0"
	);
	const auto second = Net::physical_interface_id(
		"36982b000347bb4c", "p0", "0002:01:00.0", "enP2p1s0f0np0"
	);
	const auto other_port = Net::physical_interface_id(
		"36982b000347bb4c", "p1", "0000:01:00.1", "enp1s0f1np1"
	);

	EXPECT_EQ(first, second);
	EXPECT_NE(first, other_port);
	EXPECT_EQ(Net::physical_interface_id("", "", "0000:01:00.0", "eth0"), "0000:01:00.0");
	EXPECT_EQ(Net::physical_interface_id("", "", "", "eth0"), "eth0");
}

TEST(network, graph_scales_remain_independent_without_sync) {
	auto scales = std::vector<Net::net_graph_scale> {{100, 20}, {5, 200}};
	Net::synchronize_graph_scales(scales, false, false);

	EXPECT_EQ(scales[0].download, 100);
	EXPECT_EQ(scales[0].upload, 20);
	EXPECT_EQ(scales[1].download, 5);
	EXPECT_EQ(scales[1].upload, 200);
}

TEST(network, graph_scales_can_sync_directions_only) {
	auto scales = std::vector<Net::net_graph_scale> {{100, 20}, {5, 200}};
	Net::synchronize_graph_scales(scales, true, false);

	EXPECT_EQ(scales[0].download, 100);
	EXPECT_EQ(scales[0].upload, 100);
	EXPECT_EQ(scales[1].download, 200);
	EXPECT_EQ(scales[1].upload, 200);
}

TEST(network, graph_scales_can_sync_interfaces_only) {
	auto scales = std::vector<Net::net_graph_scale> {{100, 20}, {5, 200}};
	Net::synchronize_graph_scales(scales, false, true);

	EXPECT_EQ(scales[0].download, 100);
	EXPECT_EQ(scales[0].upload, 200);
	EXPECT_EQ(scales[1].download, 100);
	EXPECT_EQ(scales[1].upload, 200);
}

TEST(network, graph_scales_can_sync_directions_and_interfaces) {
	auto scales = std::vector<Net::net_graph_scale> {{100, 20}, {5, 200}};
	Net::synchronize_graph_scales(scales, true, true);

	for (const auto& scale : scales) {
		EXPECT_EQ(scale.download, 200);
		EXPECT_EQ(scale.upload, 200);
	}
}

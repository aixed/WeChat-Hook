#pragma once

#ifndef __CPP_ORDER_JSON
#define __CPP_ORDER_JSON
#include "json.hpp"
#include "fifo_map.hpp"

// 使用fifo_map作为map的解决方案
namespace nlohmann {
	template<class Key, class Value, class dummy_compare, class trans>
	using order_workaround_fifo_map = nlohmann::fifo_map<Key, Value, nlohmann::fifo_map_compare<Key>, trans>;
	using order_json = nlohmann::basic_json<order_workaround_fifo_map>;
	using orderJson = order_json;
}
#endif
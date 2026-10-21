#ifndef HPS_ENUM_HPS_TRAITS_H
#define HPS_ENUM_HPS_TRAITS_H

/*
 * @file hps/enum/hps_traits.h
 * @brief Legacy enum traits generator preserving the original API and behavior.
 *
 * Enum traits generator for HPS.
 *
 * Goals:
 * - C89/C90 through C23 friendly style.
 * - C++ compatible.
 * - No heap allocation.
 * - No rand() dependency and no hidden global state.
 * - Works with contiguous and non-contiguous enum values.
 * - Supports declaration / implementation / header-only modes.
 */

#include "hps/base/hps_stddef.h"
#include "hps/base/hps_stdint.h"
#include "hps/base/hps_bool.h"
#include <string.h>
#include "hps/base/hps_attributes.h"

#ifdef __cplusplus
extern "C" {
#endif






#ifndef HPS_ENUM_UNKNOWN_NAME
#define HPS_ENUM_UNKNOWN_NAME "UNKNOWN"
#endif

#ifndef HPS_ENUM_EXTERNAL
#define HPS_ENUM_EXTERNAL
#endif

#ifndef HPS_ENUM_NPOS
#define HPS_ENUM_NPOS ((size_t)-1)
#endif

#define HPS_ENUM_EXPAND_VALUE(enum_value, enum_name) enum_value,
#define HPS_ENUM_EXPAND_NAME(enum_value, enum_name) enum_name,

	/*======================================================================================================================
	 * Small generic helpers
	 *====================================================================================================================*/

	static HPS_INLINE size_t hps_enum_cstr_length(const char* str)
	{
		const char* p;

		if (str == 0) {
			return 0u;
		}

		p = str;

		while (*p != '\0') {
			p++;
		}

		return (size_t)(p - str);
	}

	static HPS_INLINE bool hps_enum_cstr_is_empty(const char* str)
	{
		return (str == 0) || (str[0] == '\0');
	}

	static HPS_INLINE bool hps_enum_cstr_equal(const char* left, const char* right)
	{
		size_t left_len;
		size_t right_len;

		if ((left == 0) || (right == 0)) {
			return false;
		}

		left_len = hps_enum_cstr_length(left);
		right_len = hps_enum_cstr_length(right);

		if (left_len != right_len) {
			return false;
		}

		return memcmp(left, right, left_len) == 0;
	}

	static HPS_INLINE char hps_enum_ascii_to_lower(char ch)
	{
		if ((ch >= 'A') && (ch <= 'Z')) {
			return (char)(ch - 'A' + 'a');
		}

		return ch;
	}

	static HPS_INLINE char hps_enum_ascii_to_upper(char ch)
	{
		if ((ch >= 'a') && (ch <= 'z')) {
			return (char)(ch - 'a' + 'A');
		}

		return ch;
	}

	static HPS_INLINE bool hps_enum_cstr_equal_ignore_case(const char* left, const char* right)
	{
		size_t i;

		if ((left == 0) || (right == 0)) {
			return false;
		}

		i = 0u;

		while ((left[i] != '\0') && (right[i] != '\0')) {
			if (hps_enum_ascii_to_lower(left[i]) !=
				hps_enum_ascii_to_lower(right[i])) {
				return false;
			}

			i++;
		}

		return left[i] == right[i];
	}

	static HPS_INLINE bool hps_enum_cstr_copy(char* out, size_t out_capacity, const char* src)
	{
		size_t len;

		if ((out == 0) || (out_capacity == 0u) || (src == 0)) {
			return false;
		}

		len = hps_enum_cstr_length(src);

		if (len >= out_capacity) {
			out[0] = '\0';
			return false;
		}

		memcpy(out, src, len);
		out[len] = '\0';

		return true;
	}

	static HPS_INLINE bool hps_enum_cstr_clear(char* out, size_t out_capacity)
	{
		if ((out == 0) || (out_capacity == 0u)) {
			return false;
		}

		out[0] = '\0';

		return true;
	}

	static HPS_INLINE bool hps_enum_cstr_append(char* out, size_t out_capacity, const char* src)
	{
		size_t out_len;
		size_t src_len;

		if ((out == 0) || (out_capacity == 0u) || (src == 0)) {
			return false;
		}

		out_len = hps_enum_cstr_length(out);
		src_len = hps_enum_cstr_length(src);

		if ((out_len + src_len) >= out_capacity) {
			out[out_capacity - 1u] = '\0';
			return false;
		}

		memcpy(out + out_len, src, src_len);
		out[out_len + src_len] = '\0';

		return true;
	}

	static HPS_INLINE bool hps_enum_cstr_append_char(char* out, size_t out_capacity, char ch)
	{
		size_t out_len;

		if ((out == 0) || (out_capacity == 0u)) {
			return false;
		}

		out_len = hps_enum_cstr_length(out);

		if ((out_len + 1u) >= out_capacity) {
			out[out_capacity - 1u] = '\0';
			return false;
		}

		out[out_len] = ch;
		out[out_len + 1u] = '\0';

		return true;
	}

	static HPS_INLINE bool hps_enum_format_name(const char* prefix_name,
		const char* enum_name,
		char* out,
		size_t out_capacity)
	{
		if ((prefix_name == 0) || (enum_name == 0) ||
			(out == 0) || (out_capacity == 0u)) {
			return false;
		}

		out[0] = '\0';

		if (!hps_enum_cstr_append(out, out_capacity, "[")) {
			return false;
		}

		if (!hps_enum_cstr_append(out, out_capacity, prefix_name)) {
			return false;
		}

		if (!hps_enum_cstr_append(out, out_capacity, ": ")) {
			return false;
		}

		if (!hps_enum_cstr_append(out, out_capacity, enum_name)) {
			return false;
		}

		if (!hps_enum_cstr_append(out, out_capacity, "]")) {
			return false;
		}

		return true;
	}

	/*======================================================================================================================
	 * Declaration generator
	 *====================================================================================================================*/


#define HPS_ENUM_DECLARE_WITH_LINKAGE(enum_type, prefix, linkage) \
	linkage const char *prefix##_to_name(enum_type value); \
	linkage const char *prefix##_to_cstr(enum_type value); \
	linkage bool prefix##_to_string(enum_type value, char *out, size_t out_capacity); \
	linkage bool prefix##_format(enum_type value, char *out, size_t out_capacity); \
	linkage bool prefix##_from_name(const char *name, enum_type *out_value); \
	linkage bool prefix##_from_cstr(const char *name, enum_type *out_value); \
	linkage bool prefix##_from_name_ignore_case(const char *name, enum_type *out_value); \
	linkage bool prefix##_from_i32(int32_t raw_value, enum_type *out_value); \
	linkage bool prefix##_try_parse(const char *name, enum_type *out_value); \
	linkage bool prefix##_try_parse_ignore_case(const char *name, enum_type *out_value); \
	linkage bool prefix##_is_valid(enum_type value); \
	linkage bool prefix##_is_defined(enum_type value); \
	linkage bool prefix##_contains(enum_type value); \
	linkage bool prefix##_is_unknown(enum_type value); \
	linkage bool prefix##_is_first(enum_type value); \
	linkage bool prefix##_is_last(enum_type value); \
	linkage size_t prefix##_count(void); \
	linkage size_t prefix##_values(enum_type *out_values, size_t out_capacity); \
	linkage bool prefix##_value_at(size_t index, enum_type *out_value); \
	linkage const char *prefix##_name_at(size_t index); \
	linkage bool prefix##_index_of(enum_type value, size_t *out_index); \
	linkage enum_type prefix##_first(void); \
	linkage enum_type prefix##_last(void); \
	linkage enum_type prefix##_min_value(void); \
	linkage enum_type prefix##_max_value(void); \
	linkage bool prefix##_next(enum_type value, enum_type *out_value); \
	linkage bool prefix##_previous(enum_type value, enum_type *out_value); \
	linkage bool prefix##_next_wrap(enum_type value, enum_type *out_value); \
	linkage bool prefix##_previous_wrap(enum_type value, enum_type *out_value); \
	linkage const char *prefix##_safe_name(enum_type value, const char *fallback_name); \
	linkage bool prefix##_distance(enum_type left, enum_type right, int32_t *out_distance); \
	linkage bool prefix##_for_each(bool (*callback)(enum_type value, const char *name, void *ctx), void *ctx); \
	linkage int32_t prefix##_to_i32(enum_type value); \
	linkage int prefix##_compare(enum_type left, enum_type right);

#define HPS_ENUM_DECLARE(enum_type, prefix, item_list) \
	HPS_ENUM_DECLARE_WITH_LINKAGE(enum_type, prefix, HPS_ENUM_EXTERNAL)

	 /*======================================================================================================================
	  * Implementation generator
	  *====================================================================================================================*/

#define HPS_ENUM_DEFINE_METHODS_WITH_LINKAGE(enum_type, prefix, item_list, linkage) \
	linkage const char *prefix##_to_name(enum_type value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		static const char *names[] = { item_list(HPS_ENUM_EXPAND_NAME) }; \
		size_t count; \
		size_t i; \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (values[i] == value) { \
				return names[i]; \
			} \
		} \
		return HPS_ENUM_UNKNOWN_NAME; \
	} \
	linkage const char *prefix##_to_cstr(enum_type value) \
	{ \
		return prefix##_to_name(value); \
	} \
	linkage bool prefix##_to_string(enum_type value, char *out, size_t out_capacity) \
	{ \
		return hps_enum_cstr_copy(out, out_capacity, prefix##_to_name(value)); \
	} \
	linkage bool prefix##_format(enum_type value, char *out, size_t out_capacity) \
	{ \
		return hps_enum_format_name(#prefix, prefix##_to_name(value), out, out_capacity); \
	} \
	linkage bool prefix##_from_name(const char *name, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		static const char *names[] = { item_list(HPS_ENUM_EXPAND_NAME) }; \
		size_t count; \
		size_t i; \
		if ((name == 0) || (out_value == 0)) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (hps_enum_cstr_equal(name, names[i])) { \
				*out_value = values[i]; \
				return true; \
			} \
		} \
		return false; \
	} \
	linkage bool prefix##_from_cstr(const char *name, enum_type *out_value) \
	{ \
		return prefix##_from_name(name, out_value); \
	} \
	linkage bool prefix##_from_name_ignore_case(const char *name, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		static const char *names[] = { item_list(HPS_ENUM_EXPAND_NAME) }; \
		size_t count; \
		size_t i; \
		if ((name == 0) || (out_value == 0)) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (hps_enum_cstr_equal_ignore_case(name, names[i])) { \
				*out_value = values[i]; \
				return true; \
			} \
		} \
		return false; \
	} \
	linkage bool prefix##_from_i32(int32_t raw_value, enum_type *out_value) \
	{ \
		enum_type value; \
		if (out_value == 0) { \
			return false; \
		} \
		value = (enum_type)raw_value; \
		if (!prefix##_is_valid(value)) { \
			return false; \
		} \
		*out_value = value; \
		return true; \
	} \
	linkage bool prefix##_try_parse(const char *name, enum_type *out_value) \
	{ \
		return prefix##_from_name(name, out_value); \
	} \
	linkage bool prefix##_try_parse_ignore_case(const char *name, enum_type *out_value) \
	{ \
		return prefix##_from_name_ignore_case(name, out_value); \
	} \
	linkage bool prefix##_is_valid(enum_type value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t i; \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (values[i] == value) { \
				return true; \
			} \
		} \
		return false; \
	} \
	linkage bool prefix##_is_defined(enum_type value) \
	{ \
		return prefix##_is_valid(value); \
	} \
	linkage bool prefix##_contains(enum_type value) \
	{ \
		return prefix##_is_valid(value); \
	} \
	linkage bool prefix##_is_unknown(enum_type value) \
	{ \
		return !prefix##_is_valid(value); \
	} \
	linkage bool prefix##_is_first(enum_type value) \
	{ \
		return prefix##_is_valid(value) && (value == prefix##_first()); \
	} \
	linkage bool prefix##_is_last(enum_type value) \
	{ \
		return prefix##_is_valid(value) && (value == prefix##_last()); \
	} \
	linkage size_t prefix##_count(void) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		return sizeof(values) / sizeof(values[0]); \
	} \
	linkage size_t prefix##_values(enum_type *out_values, size_t out_capacity) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t copy_count; \
		size_t i; \
		if ((out_values == 0) || (out_capacity == 0u)) { \
			return 0u; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		copy_count = (out_capacity < count) ? out_capacity : count; \
		for (i = 0u; i < copy_count; i++) { \
			out_values[i] = values[i]; \
		} \
		return copy_count; \
	} \
	linkage bool prefix##_value_at(size_t index, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		if (out_value == 0) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		if (index >= count) { \
			return false; \
		} \
		*out_value = values[index]; \
		return true; \
	} \
	linkage const char *prefix##_name_at(size_t index) \
	{ \
		static const char *names[] = { item_list(HPS_ENUM_EXPAND_NAME) }; \
		size_t count; \
		count = sizeof(names) / sizeof(names[0]); \
		if (index >= count) { \
			return HPS_ENUM_UNKNOWN_NAME; \
		} \
		return names[index]; \
	} \
	linkage bool prefix##_index_of(enum_type value, size_t *out_index) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t i; \
		if (out_index != 0) { \
			*out_index = HPS_ENUM_NPOS; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (values[i] == value) { \
				if (out_index != 0) { \
					*out_index = i; \
				} \
				return true; \
			} \
		} \
		return false; \
	} \
	linkage enum_type prefix##_first(void) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		return values[0]; \
	} \
	linkage enum_type prefix##_last(void) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		count = sizeof(values) / sizeof(values[0]); \
		return values[count - 1u]; \
	} \
	linkage enum_type prefix##_min_value(void) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t i; \
		enum_type min_value; \
		count = sizeof(values) / sizeof(values[0]); \
		min_value = values[0]; \
		for (i = 1u; i < count; i++) { \
			if ((int32_t)values[i] < (int32_t)min_value) { \
				min_value = values[i]; \
			} \
		} \
		return min_value; \
	} \
	linkage enum_type prefix##_max_value(void) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t i; \
		enum_type max_value; \
		count = sizeof(values) / sizeof(values[0]); \
		max_value = values[0]; \
		for (i = 1u; i < count; i++) { \
			if ((int32_t)values[i] > (int32_t)max_value) { \
				max_value = values[i]; \
			} \
		} \
		return max_value; \
	} \
	linkage bool prefix##_next(enum_type value, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t index; \
		if (out_value == 0) { \
			return false; \
		} \
		if (!prefix##_index_of(value, &index)) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		if ((index + 1u) >= count) { \
			return false; \
		} \
		*out_value = values[index + 1u]; \
		return true; \
	} \
	linkage bool prefix##_previous(enum_type value, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t index; \
		if (out_value == 0) { \
			return false; \
		} \
		if (!prefix##_index_of(value, &index)) { \
			return false; \
		} \
		if (index == 0u) { \
			return false; \
		} \
		*out_value = values[index - 1u]; \
		return true; \
	} \
	linkage bool prefix##_next_wrap(enum_type value, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t index; \
		if (out_value == 0) { \
			return false; \
		} \
		if (!prefix##_index_of(value, &index)) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		*out_value = values[(index + 1u) % count]; \
		return true; \
	} \
	linkage bool prefix##_previous_wrap(enum_type value, enum_type *out_value) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		size_t count; \
		size_t index; \
		if (out_value == 0) { \
			return false; \
		} \
		if (!prefix##_index_of(value, &index)) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		if (index == 0u) { \
			*out_value = values[count - 1u]; \
		} else { \
			*out_value = values[index - 1u]; \
		} \
		return true; \
	} \
	linkage const char *prefix##_safe_name(enum_type value, const char *fallback_name) \
	{ \
		if (prefix##_is_valid(value)) { \
			return prefix##_to_name(value); \
		} \
		return (fallback_name != 0) ? fallback_name : HPS_ENUM_UNKNOWN_NAME; \
	} \
	linkage bool prefix##_distance(enum_type left, enum_type right, int32_t *out_distance) \
	{ \
		size_t left_index; \
		size_t right_index; \
		if (out_distance == 0) { \
			return false; \
		} \
		if (!prefix##_index_of(left, &left_index)) { \
			return false; \
		} \
		if (!prefix##_index_of(right, &right_index)) { \
			return false; \
		} \
		*out_distance = (int32_t)right_index - (int32_t)left_index; \
		return true; \
	} \
	linkage bool prefix##_for_each(bool (*callback)(enum_type value, const char *name, void *ctx), void *ctx) \
	{ \
		static const enum_type values[] = { item_list(HPS_ENUM_EXPAND_VALUE) }; \
		static const char *names[] = { item_list(HPS_ENUM_EXPAND_NAME) }; \
		size_t count; \
		size_t i; \
		if (callback == 0) { \
			return false; \
		} \
		count = sizeof(values) / sizeof(values[0]); \
		for (i = 0u; i < count; i++) { \
			if (!callback(values[i], names[i], ctx)) { \
				return false; \
			} \
		} \
		return true; \
	} \
	\
	linkage int32_t prefix##_to_i32(enum_type value) \
	{ \
		return (int32_t)value; \
	} \
	\
	linkage int prefix##_compare(enum_type left, enum_type right) \
	{ \
		int32_t left_value; \
		int32_t right_value; \
		left_value = (int32_t)left; \
		right_value = (int32_t)right; \
		if (left_value < right_value) { \
			return -1; \
		} \
		if (left_value > right_value) { \
			return 1; \
		} \
		return 0; \
	}

#define HPS_ENUM_IMPLEMENT(enum_type, prefix, item_list) \
	HPS_ENUM_DEFINE_METHODS_WITH_LINKAGE(enum_type, prefix, item_list, HPS_ENUM_EXTERNAL)


#define HPS_ENUM_DEFINE_INLINE(enum_type, prefix, item_list) \
	HPS_ENUM_DECLARE_WITH_LINKAGE(enum_type, prefix, static HPS_INLINE) \
	HPS_ENUM_DEFINE_METHODS_WITH_LINKAGE(enum_type, prefix, item_list, static HPS_INLINE)

#ifdef __cplusplus
}
#endif

#endif /* HPS_ENUM_HPS_TRAITS_H */

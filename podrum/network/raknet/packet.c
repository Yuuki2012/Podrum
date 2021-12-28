/*
                   Podrum R3 Copyright MFDGaming & PodrumTeam
                 This file is licensed under the GPLv2 license.
              To use this file you must own a copy of the license.
                       If you do not you can get it from:
            http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html
 */

#include "./packet.h"

packet_unconnected_ping_t get_packet_unconnected_ping(binary_stream_t *stream)
{
	packet_unconnected_ping_t packet;
	++stream->offset; // PACKET_ID
	packet.timestamp = get_unsigned_long_be(stream);
	stream->offset += 16; // MAGIC
	packet.guid = get_unsigned_long_be(stream);
	return packet;
}

packet_unconnected_pong_t get_packet_unconnected_pong(binary_stream_t *stream)
{
	packet_unconnected_pong_t packet;
	++stream->offset; // PACKET_ID
	packet.timestamp = get_unsigned_long_be(stream);
	packet.guid = get_unsigned_long_be(stream);
	stream->offset += 16; // MAGIC
	unsigned short length = get_unsigned_short_be(stream);
	packet.message = malloc(length + 1);
	int i;
	for (i = 0; i < length; ++i) {
		packet.message[i] = get_unsigned_byte_be(stream);
	}
	packet.message[i] = 0x00;
	return packet;
}

packet_incompatible_protocol_version_t get_packet_incompatible_protocol_version(binary_stream_t *stream)
{
	packet_incompatible_protocol_version_t packet;
	++stream->offset; // PACKET_ID
	packet.protocol_version = get_unsigned_byte(stream);
	stream->offset += 16; // MAGIC
	packet.guid = get_unsigned_long_be(stream);
	return packet;
}

packet_open_connection_request_1_t get_packet_open_connection_request_1(binary_stream_t *stream)
{
	packet_open_connection_request_1_t packet;
	stream->offset += 17; // PACKET_ID + MAGIC
	packet.protocol_version = get_unsigned_byte(stream);
	stream->offset = stream->size - 1;
	packet.mtu_size = stream->size; // PACKET_ID + MAGIC + PROTOCOL_VERSION + PAD_BYTES
	return packet;
}

packet_open_connection_reply_1_t get_packet_open_connection_reply_1(binary_stream_t *stream)
{
	packet_open_connection_reply_1_t packet;
	stream->offset += 17; // PACKET_ID + MAGIC
	packet.guid = get_unsigned_long_be(stream);
	packet.use_security = get_unsigned_byte(stream);
	packet.mtu_size = get_unsigned_short_be(stream);
	return packet;
}

packet_open_connection_request_2_t get_packet_open_connection_request_2(binary_stream_t *stream)
{
	packet_open_connection_request_2_t packet;
	stream->offset += 17; // PACKET_ID + MAGIC
	packet.address = get_misc_address(stream);
	packet.mtu_size = get_unsigned_short_be(stream);
	packet.guid = get_unsigned_long_be(stream);
	return packet;
}

packet_open_connection_reply_2_t get_packet_open_connection_reply_2(binary_stream_t *stream)
{
	packet_open_connection_reply_2_t packet;
	stream->offset += 17; // PACKET_ID + MAGIC
	packet.guid = get_unsigned_long_be(stream);
	packet.address = get_misc_address(stream);
	packet.mtu_size = get_unsigned_short_be(stream);
	packet.use_encryption = get_unsigned_byte(stream);
	return packet;
}

packet_acknowledge_t get_packet_acknowledge(binary_stream_t *stream)
{
	packet_acknowledge_t packet;
	++stream->offset; // PACKET_ID
	packet.sequence_numbers_count = 0;
	packet.sequence_numbers = malloc(0);
	unsigned short record_count = get_unsigned_short_be(stream);
	int i;
	unsigned char is_single;
	int index;
	int end_index;
	for (i = 0; i < record_count; ++i) {
		is_single = get_unsigned_byte(stream);
		if (is_single != 0) {
			++packet.sequence_numbers_count;
			packet.sequence_numbers = realloc(packet.sequence_numbers, packet.sequence_numbers_count * sizeof(long));
		 	packet.sequence_numbers[packet.sequence_numbers_count - 1] = get_unsigned_triad_le(stream);
		} else {
			index = get_unsigned_triad_le(stream);
			end_index = get_unsigned_triad_le(stream);
			packet.sequence_numbers = realloc(packet.sequence_numbers, (packet.sequence_numbers_count + (end_index - index + 1)) * sizeof(long));
			while (index <= end_index) {
				packet.sequence_numbers[packet.sequence_numbers_count] = index;
				++packet.sequence_numbers_count;
				++index;
			}
		}
	}
	return packet;
}

packet_frame_set_t get_packet_frame_set(binary_stream_t *stream)
{
	packet_frame_set_t packet;
	++stream->offset; // PACKET_ID
	packet.sequence_number = get_unsigned_triad_le(stream);
	packet.frames_count = 0;
	packet.frames = malloc(0);
	while (stream->offset < stream->size)
	{
		++packet.frames_count;
		packet.frames = realloc(packet.frames, packet.frames_count * sizeof(misc_frame_t));
		packet.frames[packet.frames_count - 1] = get_misc_frame(stream);
	}
	return packet;
}

packet_connection_request_t get_packet_connection_request(binary_stream_t *stream)
{
	packet_connection_request_t packet;
	++stream->offset; // PACKET_ID
	packet.guid = get_unsigned_long_be(stream);
	packet.timestamp = get_unsigned_long_be(stream);
	return packet;
}

packet_connection_request_accepted_t get_packet_connection_request_accepted(binary_stream_t *stream);

packet_new_incoming_connection_t get_packet_new_incoming_connection(binary_stream_t *stream);

packet_connected_ping_t get_packet_connected_ping(binary_stream_t *stream);

packet_connected_pong_t get_packet_connected_pong(binary_stream_t *stream);

void put_packet_unconnected_ping(packet_unconnected_ping_t packet, binary_stream_t *stream);

void put_packet_unconnected_pong(packet_unconnected_pong_t packet, binary_stream_t *stream);

void put_packet_incompatible_protocol_version(packet_incompatible_protocol_version_t packet, binary_stream_t *stream);

void put_packet_open_connection_request_1(packet_open_connection_request_1_t packet, binary_stream_t *stream);

void put_packet_open_connection_reply_1(packet_open_connection_reply_1_t packet, binary_stream_t *stream);

void put_packet_open_connection_request_2(packet_open_connection_request_1_t packet, binary_stream_t *stream);

void put_packet_open_connection_reply_2(packet_open_connection_reply_1_t packet, binary_stream_t *stream);

void put_packet_acknowledge(packet_acknowledge_t packet, int lost, binary_stream_t *stream);

void put_packet_frame_set(packet_frame_set_t packet, binary_stream_t *stream);

void put_packet_connection_request(packet_connection_request_t packet, binary_stream_t *stream);

void put_packet_connection_request_accepted(packet_connection_request_accepted_t packet, binary_stream_t *stream);

void put_packet_new_incoming_connection(packet_new_incoming_connection_t packet, binary_stream_t *stream);

void put_packet_connected_ping(packet_connected_ping_t packet, binary_stream_t *stream);

void put_packet_connected_pong(packet_connected_pong_t packet, binary_stream_t *stream);
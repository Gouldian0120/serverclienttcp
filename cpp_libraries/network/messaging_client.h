#pragma once

#include "container.h"
#include "thread_pool.h"
#include "data_handling.h"
#include "session_types.h"

#include <map>
#include <memory>
#include <string>
#include <functional>

#include <thread>
#include "asio.hpp"

namespace network
{
	class messaging_client : public std::enable_shared_from_this<messaging_client>, data_handling
	{
	public:
		messaging_client(const std::wstring& source_id);
		~messaging_client(void);

	public:
		std::shared_ptr<messaging_client> get_ptr(void);

	public:
		std::wstring source_id(void) const;
		std::wstring source_sub_id(void) const;

	public:
		void set_auto_echo(const bool& auto_echo, const unsigned short& echo_interval);
		void set_bridge_line(const bool& bridge_line);
		void set_compress_mode(const bool& compress_mode);
		void set_session_types(const session_types& session_type);
		void set_connection_key(const std::wstring& connection_key);
		void set_snipping_targets(const std::vector<std::wstring>& snipping_targets);

	public:
		void set_connection_notification(const std::function<void(const std::wstring&, const std::wstring&, const bool&)>& notification);
		void set_message_notification(const std::function<void(std::shared_ptr<container::value_container>)>& notification);
		void set_file_notification(const std::function<void(const std::wstring&, const std::wstring&, const std::wstring&, const std::wstring&)>& notification);
		void set_binary_notification(const std::function<void(const std::wstring&, const std::wstring&, const std::wstring&, const std::wstring&, const std::vector<unsigned char>&)>& notification);

	public:
		bool is_confirmed(void) const;
		void start(const std::wstring& ip, const unsigned short& port, const unsigned short& high_priority = 8, const unsigned short& normal_priority = 8, const unsigned short& low_priority = 8);
		void stop(void);

	public:
		void echo(void);
		void send(const container::value_container& message);
		void send(std::shared_ptr<container::value_container> message);
		void send_files(const container::value_container& message);
		void send_files(std::shared_ptr<container::value_container> message);
		void send_binary(const std::wstring target_id, const std::wstring& target_sub_id, const std::vector<unsigned char>& data);

	protected:
		void send_connection(void);
		void receive_on_tcp(const data_modes& data_mode, const std::vector<unsigned char>& data) override;
		void disconnected(void) override;

		// packet
	private:
		bool compress_packet(const std::vector<unsigned char>& data);
		bool encrypt_packet(const std::vector<unsigned char>& data);
		bool send_packet(const std::vector<unsigned char>& data);

	private:
		bool decompress_packet(const std::vector<unsigned char>& data);
		bool decrypt_packet(const std::vector<unsigned char>& data);
		bool receive_packet(const std::vector<unsigned char>& data);

		// file
	private:
		bool load_file_packet(const std::vector<unsigned char>& data);
		bool compress_file_packet(const std::vector<unsigned char>& data);
		bool encrypt_file_packet(const std::vector<unsigned char>& data);
		bool send_file_packet(const std::vector<unsigned char>& data);

	private:
		bool decompress_file_packet(const std::vector<unsigned char>& data);
		bool decrypt_file_packet(const std::vector<unsigned char>& data);
		bool receive_file_packet(const std::vector<unsigned char>& data);
		bool notify_file_packet(const std::vector<unsigned char>& data);

		// binary
	private:
		bool compress_binary_packet(const std::vector<unsigned char>& data);
		bool encrypt_binary_packet(const std::vector<unsigned char>& data);
		bool send_binary_packet(const std::vector<unsigned char>& data);

	private:
		bool decompress_binary_packet(const std::vector<unsigned char>& data);
		bool decrypt_binary_packet(const std::vector<unsigned char>& data);
		bool receive_binary_packet(const std::vector<unsigned char>& data);

	private:
		bool normal_message(std::shared_ptr<container::value_container> message);
		bool confirm_message(std::shared_ptr<container::value_container> message);
		bool echo_message(std::shared_ptr<container::value_container> message);

	private:
		void connection_notification(const bool& condition);

	private:
		bool _confirm;
		bool _auto_echo;
		bool _bridge_line;
		session_types _session_type;
		std::wstring _source_id;
		std::wstring _source_sub_id;
		std::wstring _target_id;
		std::wstring _target_sub_id;
		std::wstring _connection_key;
		unsigned short _auto_echo_interval_seconds;
		std::vector<std::wstring> _snipping_targets;

	private:
		bool _compress_mode;
		bool _encrypt_mode;
		std::wstring _key;
		std::wstring _iv;

	private:
		std::function<void(const std::wstring&, const std::wstring&, const bool&)> _connection;
		std::function<void(std::shared_ptr<container::value_container>)> _received_message;
		std::function<void(const std::wstring&, const std::wstring&, const std::wstring&, const std::wstring&)> _received_file;
		std::function<void(const std::wstring&, const std::wstring&, const std::wstring&, const std::wstring&, const std::vector<unsigned char>&)> _received_data;

	private:
		std::thread _thread;
		std::shared_ptr<asio::io_context> _io_context;
		std::shared_ptr<asio::ip::tcp::socket> _socket;

	private:
		std::shared_ptr<threads::thread_pool> _thread_pool;
		std::map<std::wstring, std::function<bool(std::shared_ptr<container::value_container>)>> _message_handlers;
	};
}

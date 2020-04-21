namespace process
{
	namespace detail
	{
		using uq_handle = std::unique_ptr<void, decltype(&CloseHandle)>;

		uq_handle _process_handle = { nullptr, nullptr };
		//uint64_t _base_address = 0;

		enum pattern_type
		{
			pattern_raw = 0,
			pattern_mov
		};

		std::pair<std::vector<uint8_t>, std::string> ida_to_bytes(const std::string_view signature)
		{
			std::vector<std::uint8_t> buffer{};

			// now use std::getline to seperate the string into tokens based on spaces
			std::vector<std::string> total_tokens{};
			std::string curr_token{};
			std::istringstream str_stream{ signature.data() };

			while (std::getline(str_stream, curr_token, ' '))
				total_tokens.push_back(curr_token);

			buffer.resize(total_tokens.size());

			// convert the newly seperated string into bytes (wildcards will just be zero)
			std::transform(total_tokens.cbegin(), total_tokens.cend(), buffer.begin(), [](const std::string& curr_token) -> uint8_t
				{
					return curr_token.find('?') == std::string::npos ? std::stoi(curr_token, nullptr, 16) : 0;
				});

			// since im too lazy to handle this in the other function, just make a mask using pretty much the same method.
			std::string signature_mask{};
			signature_mask.resize(buffer.size());

			std::transform(buffer.cbegin(), buffer.cend(), signature_mask.begin(), [](const uint8_t curr_byte) -> uint8_t
				{
					return curr_byte == 0 ? '?' : 'x';
				});

			return { buffer, signature_mask };
		}

		bool pattern_scan_helper(const uint8_t* data, const uint8_t* signature, const char* mask)
		{
			for (; *mask; ++mask, ++data, ++signature)
				if (*mask == 'x' && *data != *signature)
					return false;

			return (*mask) == 0;
		}
	}

	template <detail::pattern_type type = detail::pattern_mov>
	uint64_t pattern_scan(const std::string_view signature, int size, int ins)
	{
		const auto ida_signature = detail::ida_to_bytes(signature);

		const auto uq_buffer = std::make_unique<std::array<uint8_t, 0x100000ull>>();
		const auto buffer_data = uq_buffer->data();

		//
		// :)
		//
		for (auto i = 0ull; i < 0x4000000ull; i++)
		{
			if (!readto(baseAddress + i * 0x100000ull, buffer_data, 0x100000ull) || !buffer_data)
				continue;

			for (auto j = 0ull; j < 0x100000ull; j++)
			{
				if (!detail::pattern_scan_helper(buffer_data + j, reinterpret_cast<const uint8_t*>(ida_signature.first.data()), ida_signature.second.c_str()))
					continue;

				auto result = baseAddress + i * 0x100000ull + j;


				switch (type)
				{
				case detail::pattern_raw:
					//
					//
					//
					break;
				case detail::pattern_mov:
				{
					result = read_vmem(result + read_vmem<int32_t>(result + size) + ins);
					break;
				}
				default:break;
				}

				return result;
			}
		}

		return 0;
	}
}


//namespace Initialize
//{
//	uint64_t profile_manager = 0;
//	uint64_t round_manager = 0;
//	DWORD_PTR view_matrix = 0;
//	bool initialize()
//	{
//		profile_manager = process::pattern_scan("48 8B 0D ? ? ? ? E8 ? ? ? ? 80 BF ? ? ? ? ?", 3, 7);
//
//		if (!profile_manager)
//		{
//			std::cout << "[!] profile manager is outdated\n";
//			return false;
//		}
//
//		std::cout << "[+] profile manager: 0x" << std::hex << profile_manager << '\n';
//
//		round_manager = process::pattern_scan("F3 0F 10 05 ? ? ? ? 48 8D 54 24 ? F3 0F 59 C7 ", 4, 8);
//
//		if (!round_manager)
//		{
//			std::cout << "[!] round manager is outdated\n";
//			return false;
//		}
//
//		std::cout << "[+] round manager: 0x" << std::hex << round_manager << '\n';
//
//		//printf("round -> %d\n", game_state);
//
//		return true;
//	}
//}


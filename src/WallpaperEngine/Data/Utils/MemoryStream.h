#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <algorithm>

namespace WallpaperEngine::Data::Utils {
struct MemoryStream : std::istream, private std::streambuf {
    MemoryStream (std::unique_ptr<char[]> buffer, const size_t size) :
	std::istream (this), m_buffer (std::move (buffer)), m_size (size) {
	this->setg (this->m_buffer.get (), this->m_buffer.get (), this->m_buffer.get () + size);
    }

    std::streambuf::pos_type
    seekoff (std::streambuf::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override {
	if (dir == std::ios_base::cur) {
	    gbump ((int) off);
	} else if (dir == std::ios_base::end) {
	    setg (eback (), egptr () + off, egptr ());
	} else if (dir == std::ios_base::beg) {
	    setg (eback (), eback () + off, egptr ());
	}
	return gptr () - eback ();
    }

    // Windows 移植修正：默认 xsgetn 逐字符读取会触发 underflow/状态异常导致数据错位。
    std::streamsize xsgetn (char* s, std::streamsize n) override {
	const std::streamsize avail = static_cast<std::streamsize> (this->egptr () - this->gptr ());
	if (avail <= 0 || n <= 0) {
	    return 0;
	}
	const std::streamsize toCopy = std::min (n, avail);
	std::memcpy (s, this->gptr (), static_cast<size_t> (toCopy));
	this->gbump (static_cast<int> (toCopy));
	return toCopy;
    }

    std::streamsize showmanyc () override {
	return static_cast<std::streamsize> (this->egptr () - this->gptr ());
    }

    std::streambuf::int_type underflow () override {
	if (this->gptr () < this->egptr ()) {
	    return traits_type::to_int_type (*this->gptr ());
	}
	return traits_type::eof ();
    }

    std::streambuf::int_type uflow () override {
	if (this->gptr () < this->egptr ()) {
	    const char c = *this->gptr ();
	    this->gbump (1);
	    return traits_type::to_int_type (c);
	}
	return traits_type::eof ();
    }

    std::unique_ptr<char[]> m_buffer;
    size_t m_size = 0;
};

using MemoryStreamSharedPtr = std::shared_ptr<MemoryStream>;
using MemoryStreamUniquePtr = std::unique_ptr<MemoryStream>;
}

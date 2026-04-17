#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <lab4/resource/resource.hpp>
#include <unistd.h>
#include <vector>
using namespace std;
namespace lab4::resource
{
ResourceError::ResourceError(const string& message) : runtime_error(message) {}
ResourceError::ResourceError(const char* message) : runtime_error(message) {}
bool FileHandle::is_valid() const noexcept
{
    return fd_ >= 0;
}

FileHandle::FileHandle(const string& path, int flags) : fd_(::open(path.c_str(), flags))
{
    if (fd_ < 0)
    {
        throw ResourceError("Failed to open file: " + string(strerror(errno)));
    }
}

FileHandle::FileHandle(int fd) noexcept : fd_(fd) {}

FileHandle::FileHandle(FileHandle&& other) noexcept
{
    fd_ = other.fd_;
    other.fd_ = -1;
}

FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
{
    if (this != &other)
    {
        close();
        fd_ = other.fd_;
        fd_ = -1;
    }
    return *this;
}

FileHandle::~FileHandle()
{
    close();
}

void FileHandle::close() noexcept
{
    if (is_valid())
    {
        ::close(fd_);
        fd_ = -1;
    }
}

long FileHandle::read(char* buffer, size_t count)
{
    long bytes = ::read(fd_, static_cast<void*>(buffer), count);
    if (bytes < 0)
    {
        throw ResourceError("Read error: " + string(strerror(errno)));
    }
    return bytes;
}

long FileHandle::write(const char* buffer, size_t count)
{
    long bytes = ::write(fd_, static_cast<const void*>(buffer), count);
    if (bytes < 0)
    {
        throw ResourceError("Write error: " + string(strerror(errno)));
    }
    return bytes;
}
bool FileHandle::operator==(const FileHandle& other) const
{
    return fd_ == other.fd_;
}
bool FileHandle::operator!=(const FileHandle& other) const
{
    return fd_ != other.fd_;
}
shared_ptr<FileHandle> ResourceManager::acquire(const string& path, int flags)
{
    auto it = cache_.find(path);
    if (it != cache_.end())
    {
        auto oldHandle = it->second.lock();
        if (oldHandle)
        {
            return oldHandle;
        }
        else
        {
            cache_.erase(it);
        }
    }
    auto newHandle = make_shared<FileHandle>(path, flags);
    cache_[path] = newHandle;
    return newHandle;
}

void ResourceManager::clear()
{
    cache_.clear();
}

void ResourceManager::release(const string& path)
{
    auto it = cache_.find(path);
    if (it != cache_.end() && it->second.expired())
    {
        cache_.erase(it);
    }
}

}; // namespace lab4::resource

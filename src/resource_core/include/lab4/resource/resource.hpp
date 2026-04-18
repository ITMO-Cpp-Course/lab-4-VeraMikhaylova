#pragma once
#include <cstddef>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace lab4::resource
{

class ResourceError : public std::runtime_error
{
  public:
    explicit ResourceError(const std::string& message);

    explicit ResourceError(const char* message);
};

class FileHandle
{
  public:
    explicit FileHandle(const std::string& path, int flags);
    explicit FileHandle(int fd) noexcept;

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    FileHandle(FileHandle&& other) noexcept;
    FileHandle& operator=(FileHandle&& other) noexcept;

    ~FileHandle();

    bool is_valid() const noexcept;

    long read(char* buffer, std::size_t count);
    long write(const char* buffer, std::size_t count);

    bool operator==(const FileHandle& other) const;
    bool operator!=(const FileHandle& other) const;

  private:
    int fd_;
    void close() noexcept;
};
class ResourceManager
{
  public:
    std::shared_ptr<FileHandle> acquire(const std::string& path, int flags);
    void clear();
    void release(const std::string& path);

  private:
    std::unordered_map<std::string, std::weak_ptr<FileHandle>> cache_;
};

} // namespace lab4::resource

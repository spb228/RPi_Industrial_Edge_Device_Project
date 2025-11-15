#include <gtest/gtest.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unordered_map>
#include <cstring>

// Fake FD counter
static int next_fd = 1;

// Map to keep track of FDs
static std::unordered_map<int, bool> valid_fds;

// Configure mock behavior
bool mock_open_fail = false;
bool mock_ioctl_fail = false;

extern "C" {

// Mock open()
int open(const char *pathname, int flags, ...)
{
    if (mock_open_fail)
    {
        return -1; 
    }

    int fd = next_fd++;
    valid_fds[fd] = true; 

    return fd; 
}

// Mock ioctl()
int ioctl(int fd, unsigned long request, ...)
{
    if (mock_ioctl_fail)
    {
        return -1; 
    }

    if (!valid_fds.count(fd))
    {
        return -1; 
    }

    return 0;
}

// Mock close()
int close(int fd)
{
    if (!valid_fds.count(fd))
    {
        return -1;
    }

    valid_fds.erase(fd);
    return 0;
}

} // extern "C"

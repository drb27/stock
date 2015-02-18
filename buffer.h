#ifndef BUFFER_H
#define BUFFER_H

class buffer
{
 public:
    
    /* Lifecycle Management */
    buffer(unsigned long sz);
    buffer( buffer&& );
    buffer( buffer const & ) = delete;
    buffer& operator=(buffer const &) = delete;
    virtual ~buffer();


    /* Public API */
    void reset();
    bool append(const void* pData, unsigned long sz);
    unsigned long remaining_bytes() const;
    const char* contents();

protected:
    const unsigned long size;
    char* _buffer;
    char* index;

};

#endif

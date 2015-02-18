#ifndef STOCK_FETCHER_H
#define STOCK_FETCHER_H

/**
 * Fetches a stock price from Yahoo! Finance.
 * This thread class fetches the current bid, ask prices of a stock
 */
class stock_fetcher : public worker_base
{
public:
    virtual void operator()(int);
protected:
    static size_t rx_data(void *rx_buffer, size_t size, size_t nmemb, void *local_buffer);
};

#endif

#ifndef callback_h
#define callback_h

#include "order.h"
#include "base/types.h"

namespace liquibook { namespace book {

template <class OrderPtr>
class OrderBook;

// Callback events
//   New order accept
//     - order accept
//     - fill (2) and/or quote (if not complete)
//     - depth/bbo ?
//   New order reject
//     - order reject
//   Order fill
//     - fill (2)
//     - trade
//     - quote (2)
//     - depth/bbo ?
//   Order cancelled
//     - order cancel
//     - quote
//     - depth/bbo ?
//   Order cancel reject
//     - order cancel reject
//   Order replaced
//     - order replaced
//     - fill (2) and/or quote (if not complete)
//     - depth/bbo ?
//   Order replace reject
//     - order replace reject

template <class OrderPtr = Order*>
class Callback {
public:
  typedef OrderBook<OrderPtr > TypedOrderBook;

  enum CbType {
    cb_unknown,
    cb_order_accept,
    cb_order_reject,
    cb_order_fill,
    cb_order_cancel,
    cb_order_cancel_rejected,
    cb_order_replaced,
    cb_order_replace_reject,
    cb_book_update,
    cb_depth_update,
    cb_bbo_update
  };

  Callback();

  static Callback<OrderPtr> accept(const OrderPtr& order);
  static Callback<OrderPtr> fill(const OrderPtr& order,
                                 const Quantity& qty,
                                 const Price& price,
                                 const Cost& cost);
  static Callback<OrderPtr> cancel(const OrderPtr& order);
  static Callback<OrderPtr> cancel_reject(const OrderPtr& order,
                                          const char* reason);

  CbType type_;
  OrderPtr order_;
  const char* reject_reason_;
  Price fill_price_;
  Quantity fill_qty_;
  Cost fill_cost_;
};

template <class OrderPtr>
Callback<OrderPtr> Callback<OrderPtr>::accept(const OrderPtr& order)
{
  Callback<OrderPtr> result;
  result.type_ = cb_order_accept;
  result.order_ = order;
  return result;
}

template <class OrderPtr>
Callback<OrderPtr> Callback<OrderPtr>::fill(const OrderPtr& order,
                                            const Quantity& qty,
                                            const Price& price,
                                            const Cost& cost)
{
  Callback<OrderPtr> result;
  result.type_ = cb_order_fill;
  result.order_ = order;
  result.fill_qty_ = qty;
  result.fill_price_ = price;
  result.fill_cost_ = cost;
  return result;
}

template <class OrderPtr>
Callback<OrderPtr> Callback<OrderPtr>::cancel(const OrderPtr& order)
{
  Callback<OrderPtr> result;
  result.type_ = cb_order_cancel;
  result.order_ = order;
  return result;
}

template <class OrderPtr>
Callback<OrderPtr> Callback<OrderPtr>::cancel_reject(
  const OrderPtr& order,
  const char* reason)
{
  Callback<OrderPtr> result;
  result.type_ = cb_order_cancel;
  result.order_ = order;
  result.reject_reason_ = reason;
  return result;
}

template <class OrderPtr>
Callback<OrderPtr>::Callback()
: type_(cb_unknown),
  reject_reason_(NULL),
  fill_qty_(0),
  fill_cost_(0)
{
}

} }

#endif

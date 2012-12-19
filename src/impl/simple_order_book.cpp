#include "simple_order_book.h"
#include <iostream>

namespace liquibook { namespace impl {

SimpleOrderBook::SimpleOrderBook()
: fill_id_(0)
{
}


void
SimpleOrderBook::perform_callback(SimpleCallback& cb)
{
  switch(cb.type_) {
    case SimpleCallback::cb_order_accept:
      cb.order_->accept();
      // If the order is a limit order
      if (cb.order_->is_limit()) {
        // Add to bid or ask depth
        if (cb.order_->is_buy()) {
          depth_.add_bid(cb.order_->price(), cb.order_->order_qty());
        } else {
          depth_.add_ask(cb.order_->price(), cb.order_->order_qty());
        }
      }
      break;

    case SimpleCallback::cb_order_fill:
      cb.order_->fill(cb.ref_qty_, cb.ref_cost_, ++fill_id_);
      // If the order is a limit order
      if (cb.order_->is_limit()) {
        // If this fill completed the order
        if (!cb.order_->open_qty()) {
          // If the filled order is a buy
          if (cb.order_->is_buy()) {
            if (depth_.close_bid(cb.order_->price(), cb.ref_qty_)) {
              restore_last_bid_level();
            }
          // Else the filled order is a sell
          } else {
            if (depth_.close_ask(cb.order_->price(), cb.ref_qty_)) {
              restore_last_ask_level();
            }
          }
        // Else this fill reduced the order
        } else {
          int32_t quantity_delta = -cb.ref_qty_;
          // If the reduced order is a buy
          if (cb.order_->is_buy()) {
            depth_.change_qty_bid(cb.order_->price(), quantity_delta);
          // Else the reduced order is a sell
          } else {
            depth_.change_qty_ask(cb.order_->price(), quantity_delta);
          }
        }
      }
      break;

    case SimpleCallback::cb_order_cancel:
      cb.order_->cancel();
      // If the order is a limit order
      if (cb.order_->is_limit()) {
        // If the cancelled order is a buy
        if (cb.order_->is_buy()) {
          if (depth_.close_bid(cb.order_->price(), cb.order_->open_qty())) {
            restore_last_bid_level();
          }
        // Else the cancelled order is a sell
        } else {
          if (depth_.close_ask(cb.order_->price(), cb.order_->open_qty())) {
            restore_last_ask_level();
          }
        }
      }
      break;

    case SimpleCallback::cb_order_replace:
    {
      int32_t qty_delta = cb.ref_qty_ - cb.order_->order_qty();
      // If the quantity changed and the price did not
      if (qty_delta && cb.order_->price() == cb.ref_price_) {
        if (cb.order_->is_buy()) {
          depth_.change_qty_bid(cb.order_->price(), qty_delta);
        } else {
          depth_.change_qty_ask(cb.order_->price(), qty_delta);
        }
      // Else the price changed (and perhaps quantity also)
      } else {
        Quantity new_open_qty = cb.order_->open_qty() + qty_delta;

        // Move open quantity from current level, to updated quantity 
        // at new level
        if (cb.order_->is_buy()) {
          depth_.close_bid(cb.order_->price(), cb.order_->open_qty());
          if (new_open_qty) {
            depth_.add_bid(cb.ref_price_, new_open_qty);
          }
        } else {
          depth_.close_ask(cb.order_->price(), cb.order_->open_qty());
          if (new_open_qty) {
            depth_.add_ask(cb.ref_price_, new_open_qty);
          }
        }
        // Modify the order itself
        cb.order_->replace(cb.ref_qty_, cb.ref_price_);
      }

      // Modify the order itself
      cb.order_->replace(cb.ref_qty_, cb.ref_price_);
      break;
    }
    default:
      // Nothing
      break;
  }
}

void
SimpleOrderBook::restore_last_bid_level()
{
  Price restoration_price;

  // If restoration is needed (meaning N-1 level is populated)
  if (depth_.needs_bid_restoration(restoration_price)) {
    // Restore the last remaining level
    populate_bid_depth_level_after(restoration_price, *depth_.last_bid_level());
  }
}

void
SimpleOrderBook::restore_last_ask_level()
{
  Price restoration_price;

  // If restoration is needed (meaning N-1 level is populated)
  if (depth_.needs_ask_restoration(restoration_price)) {
    // Restore the last remaining level
    populate_ask_depth_level_after(restoration_price, *depth_.last_ask_level());
  }
}

const SimpleOrderBook::SimpleDepth&
SimpleOrderBook::depth() const
{
  return depth_;
}

} } 

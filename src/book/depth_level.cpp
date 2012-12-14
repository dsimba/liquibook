#include "depth_level.h"
#include "base/types.h"
#include <stdexcept>

namespace liquibook { namespace book {

DepthLevel::DepthLevel()
: price_(INVALID_LEVEL_PRICE),
  order_count_(0),
  aggregate_qty_(0)
{
}

DepthLevel::DepthLevel(
  Price price,
  uint32_t order_count,
  Quantity aggregate_qty)
: price_(price),
  order_count_(order_count),
  aggregate_qty_(aggregate_qty)
{
}

const Price&
DepthLevel::price() const
{
  return price_;
}

void
DepthLevel::init(Price price)
{
  price_ = price;
  order_count_ = 0;
  aggregate_qty_ = 0;
}

uint32_t
DepthLevel::order_count() const
{
  return order_count_;
}


Quantity
DepthLevel::aggregate_qty() const
{
  return aggregate_qty_;
}

void
DepthLevel::add_order(Quantity qty)
{
  // Increment/increase
  ++order_count_;
  aggregate_qty_ += qty;
}

bool
DepthLevel::close_order(Quantity qty)
{
  bool empty = false;
  // If this is the last order, reset the level
  if (order_count_ == 1) {
    init(0);  // Reset
    empty = true;
  // Else, decrement/decrease
  } else {
    --order_count_;
    if (aggregate_qty_ >= qty) {
      aggregate_qty_ -= qty;
    } else {
      throw std::runtime_error("DepthLevel::remove_order "
                               "level quantity too low");
    }
  }
  return empty;
}

void
DepthLevel::increase_qty(Quantity qty)
{
  aggregate_qty_ += qty;
}

void
DepthLevel::decrease_qty(Quantity qty)
{
  aggregate_qty_ -= qty;
}

} }

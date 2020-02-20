//*********************************************************
//Randolph Voorhies, bitfield, (2014), GitHub repository, 
//https://github.com/randvoorhies/bitfield
//*********************************************************
#pragma once

#include <array>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <iostream>

namespace bitfield_private
{
  //! A helper class to determine which integer type can hold all of our bits
  /*! Note that this limits us to 64 bit bitfields! */
  template<size_t size, class enable=void> struct uintx_t { typedef void type; };
  template<size_t size> struct uintx_t<size, typename std::enable_if<(size <= 8)>::type >               { typedef uint8_t  type; };
  template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 8 && size <= 16)>::type>   { typedef uint16_t type; };
  template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 16 && size <= 32)>::type>  { typedef uint32_t type; };
  template<size_t size> struct uintx_t<size, typename std::enable_if<(size > 32 && size <= 64)>::type>  { typedef uint64_t type; };

  template<size_t parent_bits, size_t e, size_t b, bool is_const> struct range;
}


template<size_t n_bits> class bitfield
{
  static_assert(n_bits <= 64, "bitfield must be created with <= 64 bits");

  public:

    //! The native storage type
    typedef std::array<bool, n_bits> storage_t;

    //! The integral type that this bitfield can store.
    typedef typename bitfield_private::uintx_t<n_bits>::type native_type;

    //! Default constructor - set to all zeros
    bitfield()
    {
      b_.fill(false);
    }

    //! Construct from an integer value
    bitfield(native_type v)
    {
      range<n_bits-1,0>() = v;
    }

    //! Copy constructor
    bitfield(bitfield<n_bits> const & other) : b_(other.b_) { }

    //! Copy from a range
    /*! For example:
     *  @code 
     *    bitfield<4> b2 = b1.range<3,0>();
     *  @endcode */
    template<size_t o_bits, size_t e, size_t b, bool is_const>
    bitfield(bitfield_private::range<o_bits,e,b,is_const> const & other_range)
    {
      static_assert(e+1-b == n_bits, "Trying to assign range to bitfield with mismatching sizes");
      range<n_bits-1,0>() = other_range;
    }

    //! Access a range of the bitfield
    template<size_t e, size_t b>
      bitfield_private::range<n_bits,e,b,false> range() 
      { 
        static_assert(e >= b,   "bitfield<bits>::range<e,b> must be called with e >= b");
        static_assert(e < n_bits, "bitfield<bits>::range<e,b> must be called with e < bits and b");

        return bitfield_private::range<n_bits,e,b,false>(*this);
      }

    //! Access a range of the bitfield (const version)
    template<size_t e, size_t b>
      bitfield_private::range<n_bits,e,b,true> range() const
      { 
        static_assert(e >= b,   "bitfield<bits>::range<e,b> must be called with e >= b");
        static_assert(e < n_bits, "bitfield<bits>::range<e,b> must be called with e < bits and b");

        return bitfield_private::range<n_bits,e,b,true>(*this);
      }

    //! Assign a character string to the bitfield, e.g. bitset<3> mybitset; mybitset = "101";
    template<std::size_t N>
      void operator=(char const (& x) [N] ) 
      {
        range<n_bits-1,0>() = x;
      }

    //! Assign an integer value to the bitfield, e.g. bitset<8> mybitset; mybitset = 0xFA;
    void operator=(native_type v)
    {
      range<n_bits-1,0>() = v;
    }

    //! Convert the bitfield to a string for printing
    std::string to_string() const
    {
      return this->range<n_bits-1,0>().to_string();
    }

    //! Convert the bitfield to a number
    typename bitfield_private::uintx_t<n_bits>::type to_num() const
    {
      return this->range<n_bits-1,0>().to_num();
    }

    //! Access a single bit of the bitfield
    bool & operator[](size_t i)
    {
      return this->range<n_bits-1,0>()[i];
    }

    //! Access a single bit of the bitfield (const version)
    bool operator[](size_t i) const
    {
      return this->range<n_bits-1,0>()[i];
    }

    //! Reverse the bitfield in place
    /*! \todo This needs to just self-assign the range reversed */
    void reverse()
    {
      std::reverse(b_.end(), b_.begin());
    }

    //! Reverse a copy of the bitfield and return it
    /*! \todo This needs to just return the range reversed */
    bitfield<n_bits> reversed() const
    {
      bitfield<n_bits> other(*this);
      other.reverse();
      return other;
    }

  private:
    template<size_t,size_t,size_t,bool> friend struct bitfield_private::range;

    storage_t b_;
};

namespace bitfield_private
{

  template<class parent_type, bool is_const> struct parent_wrapper;
  template<class parent_type> struct parent_wrapper<parent_type, true>  { typedef parent_type const & type; };
  template<class parent_type> struct parent_wrapper<parent_type, false> { typedef parent_type & type; };

  //! A range class holds a reference to the parent bitfield, and can be used to set a range of its bits
  template<size_t parent_bits, size_t e, size_t b, bool is_const>
    struct range
    {
      //! The number of bits this range can hold
      static size_t const n_range_bits = e+1-b;

      //! The integral type that this range can store.
      typedef typename uintx_t<n_range_bits>::type native_range_type;

      //! The type of the parent (either const or not)
      typedef typename parent_wrapper<bitfield<parent_bits>, is_const>::type parent_type; 

      //! Construct from a parent
      range(parent_type parent) : parent_(parent) {}

      //! Copy constructor from 
      template<size_t other_parent_bits, size_t other_e, size_t other_b>
        range(range<other_parent_bits, other_e, other_b, true> const & other) :
          parent_(other.parent), reversed_(other.reversed_)
        { }

      //! Assign a character string to the range, e.g. mybitset.range<4,2>() = "101";
      template<std::size_t N, bool is_const_dummy = is_const>
        typename std::enable_if<is_const_dummy == false, void>::type
        operator=(char const (& x) [N] ) 
        {
          static_assert(N-1 == n_range_bits, "Wrong number of characters in range assignment");
          for(size_t i=b; i<=e; ++i) 
          {
            if(x[i-b] == '0' || x[i-b] == '1') 
              parent_[e-i] = (x[i-b] == '1');
            else 
              throw std::invalid_argument("Only 0 and 1 are allowed in assignment strings. You gave " + std::string(1, x[b-i]));
          }
        }

      //! Assign an integer value to the range, e.g. mybitset.range<7,0>() = 0xFA;
      template<bool is_const_dummy = is_const>
        typename std::enable_if<is_const_dummy == false, void>::type
        operator=(native_range_type v)
        {
          if(v > ((1 << n_range_bits) - 1))
            throw std::invalid_argument("Too large a value given to range");

          for(size_t i=b; i<=e; ++i)
          {
            parent_[i] = v & 0x01;
            v = v >> 1;
          }
        }

      //! Copy another range's values to this one
      /*! For example
       *  @code
       *   b2.range<3,0>() = b1.range<4,7>();
       *  @endcode */
      template<size_t other_parent_bits, size_t other_e, size_t other_b, bool other_is_const, bool is_const_dummy = is_const>
        typename std::enable_if<is_const_dummy == false, void>::type
        operator=(range<other_parent_bits, other_e, other_b, other_is_const> const & other)
      {
        static_assert(n_range_bits == other_e+1-other_b, "Trying to assign ranges with mismatching sizes");
        for(size_t i=0; i<n_range_bits; ++i)
          (*this)[i] = other[i];
      }

      //! Convert the bitfield range to a string for printing
      std::string to_string()
      {
        std::string s(n_range_bits, '-');
        for(size_t i=0; i<n_range_bits; ++i)
          s[n_range_bits-i-1] = (*this)[i] ? '1' : '0';
        return s;
      }

      //! Convert the bitfield to a number
      native_range_type to_num()
      {
        native_range_type n(0);
        for(size_t i=0; i<n_range_bits; ++i)
          if((*this)[i]) n += (0x01 << i);

        return n;
      }

      //! Access an element of the range
      template<bool is_const_dummy = is_const>
      typename std::enable_if<is_const_dummy == false, bool &>::type
        operator[](size_t i)
      {
        if(reversed_)
          return parent_.b_[b+i];          
        else
          return parent_.b_[e-i];
      }

      //! Access an element of the range (const version)
      bool operator[](size_t i) const
      {
        if(reversed_)
          return parent_.b_[b+i];          
        else
          return parent_.b_[e-i];
      }

      //! Reverse the bitfield in place
      /*! This will actually reverse the bits in the original bitfield*/
      template<bool is_const_dummy = is_const>
      typename std::enable_if<is_const_dummy == false, void>::type
        reverse()
        {
          std::reverse(&(*this)[n_range_bits-1], &(*this)[0]);
        }

      //! Return a "view" of the bitfield range with the bits reversed
      /*! This is a non-destructive call, and will not actually reverse any bits in the parent bitfield */
      range<parent_bits,e,b,is_const> reversed()
      {
        range<parent_bits,e,b,is_const> other = *this;
        other.reversed_ = !reversed_;
        return other;
      }

      parent_type parent_;
      bool reversed_ = false;
    };
}
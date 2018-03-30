#ifndef __CS489_FILEIO_H
#define __CS489_FILEIO_H

//
// BYTE ORDER FUNCTIONS
//
#pragma region BYTE_ORDER_FUNCTIONS

template<class T>
inline void reverse_byte_order(T* data)
{
 uint08* ptr = reinterpret_cast<uint08*>(data);
 std::reverse(ptr, ptr + sizeof(T)); 
}

template<class T>
inline void reverse_byte_order(T* data, size_t elem)
{
 for(size_t i = 0; i < elem; i++) {
     uint08* ptr = reinterpret_cast<uint08*>(&data[i]);
     std::reverse(ptr, ptr + sizeof(T));
    }
}

#pragma endregion BYTE_ORDER_FUNCTIONS

//
// READING FUNCTIONS
//
#pragma region READING_FUNCTIONS

// Binary Fundamental Types
template<class T>
inline T LE_read(std::istream& is)
{
 T temp;
 is.read((char*)&temp, sizeof(temp));
 return temp;
}

template<class T> 
inline T BE_read(std::istream& is)
{
 T temp;
 is.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

template<class T>
inline T LE_read(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return T();
 return LE_read<T>(is);
}

template<class T>
inline T BE_read(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return T();
 return BE_read<T>(is);
}

// Binary Fundamental Types (Specializations)
template<>
inline real32 BE_read<real32>(std::istream& is)
{
 uint32 temp = BE_read<uint32>(is); // read data as uint to avoid NaN conversion
 return *reinterpret_cast<real32*>(&temp);
}

template<>
inline real64 BE_read<real64>(std::istream& is)
{
 uint64 temp = BE_read<uint64>(is); // read data as uint to avoid NaN conversion
 return *reinterpret_cast<real64*>(&temp);
}

template<>
inline real32 BE_read<real32>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return real32();
 return BE_read<real32>(is); // use specialization
}

template<>
inline real64 BE_read<real64>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return real64();
 return BE_read<real64>(is);
}

// Binary Arrays
template<class T>
inline bool LE_read_array(std::istream& is, T* data, size_t n)
{
 is.read((char*)&data[0], n*sizeof(T));
 if(is.fail()) return false;
 return true;
}

template<class T>
inline bool BE_read_array(std::istream& is, T* data, size_t n)
{
 is.read((char*)&data[0], n*sizeof(T));
 if(is.fail()) return false;
 reverse_byte_order(data, n);
 return true;
}

template<class T>
inline bool LE_read_array(std::istream& is, std::streampos offset, T* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return LE_read_array(is, data, n);
}

template<class T>
inline bool BE_read_array(std::istream& is, std::streampos offset, T* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}

// Binary Arrays (Specializations)
template<>
inline bool BE_read_array(std::istream& is, real32* data, size_t n)
{
 return BE_read_array(is, reinterpret_cast<uint32*>(data), n);
}

template<>
inline bool BE_read_array(std::istream& is, real64* data, size_t n)
{
 return BE_read_array(is, reinterpret_cast<uint32*>(data), n);
}

template<>
inline bool BE_read_array(std::istream& is, std::streampos offset, real32* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}

template<>
inline bool BE_read_array(std::istream& is, std::streampos offset, real64* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}

// Binary Byte Strings
inline size_t LE_read_string(std::istream& is, sint08* data, size_t size, sint08 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     sint08 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t BE_read_string(std::istream& is, sint08* data, size_t size, sint08 delimiter = 0)
{
 return LE_read_string(is, data, size, delimiter);
}

inline size_t LE_read_string(std::istream& is, uint08* data, size_t size, uint08 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     uint08 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t BE_read_string(std::istream& is, uint08* data, size_t size, uint08 delimiter = 0)
{
 return LE_read_string(is, data, size, delimiter);
}

inline size_t LE_read_string(std::istream& is, std::streampos offset, sint08* data, size_t size, sint08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_string(is, data, size, delimiter);
}

inline size_t BE_read_string(std::istream& is, std::streampos offset, sint08* data, size_t size, sint08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_string(is, data, size, delimiter);
}

inline size_t LE_read_string(std::istream& is, std::streampos offset, uint08* data, size_t size, uint08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_string(is, data, size, delimiter);
}

inline size_t BE_read_string(std::istream& is, std::streampos offset, uint08* data, size_t size, uint08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_string(is, data, size, delimiter);
}

// Binary Unicode Strings
inline size_t LE_read_unicode_string(std::istream& is, char16* data, size_t size, char16 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char16 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t BE_read_unicode_string(std::istream& is, char16* data, size_t size, char16 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char16 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     reverse_byte_order(&item);
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t LE_read_unicode_string(std::istream& is, char32* data, size_t size, char32 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char32 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t BE_read_unicode_string(std::istream& is, char32* data, size_t size, char32 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char32 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     reverse_byte_order(&item);
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}

inline size_t LE_read_unicode_string(std::istream& is, std::streampos offset, char16* data, size_t size, char16 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_unicode_string(is, data, size, delimiter);
}

inline size_t BE_read_unicode_string(std::istream& is, std::streampos offset, char16* data, size_t size, char16 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_unicode_string(is, data, size, delimiter);
}

inline size_t LE_read_unicode_string(std::istream& is, std::streampos offset, char32* data, size_t size, char32 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_unicode_string(is, data, size, delimiter);
}

inline size_t BE_read_unicode_string(std::istream& is, std::streampos offset, char32* data, size_t size, char32 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_unicode_string(is, data, size, delimiter);
}

#pragma endregion READING_FUNCTIONS

//
// WRITING FUNCTIONS
//
#pragma region WRITING_FUNCTIONS

// Binary Fundamental Types
template<class T>
inline void LE_write(std::ostream& os, T value)
{
 os.write((char*)&value, sizeof(value));
}

template<class T>
inline void BE_write(std::ostream& os, T value)
{
 reverse_byte_order(&value);
 os.write((char*)&value, sizeof(value));
}

// Binary Fundamental Types (Specializations)
template<>
inline void BE_write(std::ostream& os, real32 value)
{
 uint32 rever = *reinterpret_cast<uint32*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
}

template<>
inline void BE_write(std::ostream& os, real64 value)
{
 uint64 rever = *reinterpret_cast<uint64*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
}

// Binary Arrays
template<class T>
inline bool LE_write_array(std::ostream& os, const T* data, size_t n)
{
 os.write((char*)data, n*sizeof(T));
 if(os.fail()) return false;
 return true;
}

template<class T>
inline bool BE_write_array(std::ostream& os, const T* data, size_t n)
{
 std::unique_ptr<T[]> copy(new T[n]);
 for(size_t i = 0; i < n; i++) copy.get()[i] = data[i];
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(T));
 if(os.fail()) return false;
 return true;
}

// Binary Arrays (Specializations)
template<>
inline bool BE_write_array(std::ostream& os, const real32* data, size_t n)
{
 std::unique_ptr<uint32[]> copy(new uint32[n]);
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint32*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(real32));
 if(os.fail()) return false;
 return true;
}

template<>
inline bool BE_write_array(std::ostream& os, const real64* data, size_t n)
{
 std::unique_ptr<uint64[]> copy(new uint64[n]);
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint64*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(real64));
 if(os.fail()) return false;
 return true;
}

// Binary Byte Strings
inline bool LE_write_string(std::ostream& os, const sint08* data, sint08 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t size = (std::char_traits<sint08>::length(data) + 1);
    os.write(reinterpret_cast<const char*>(data), size);
   }
 else {
   size_t index = 0;
   while(data[index++] != delimiter);
   os.write(reinterpret_cast<const char*>(data), index);
  }
 return !os.fail();
}

inline bool BE_write_string(std::ostream& os, const sint08* data, sint08 delimiter = 0)
{
 return LE_write_string(os, data, delimiter);
}

inline bool LE_write_string(std::ostream& os, const uint08* data, uint08 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t size = (std::char_traits<uint08>::length(data) + 1);
    os.write(reinterpret_cast<const char*>(data), size);
   }
 else {
   size_t index = 0;
   while(data[index++] != delimiter);
   os.write(reinterpret_cast<const char*>(data), index);
  }
 return !os.fail();
}

inline bool BE_write_string(std::ostream& os, const uint08* data, uint08 delimiter = 0)
{
 return LE_write_string(os, data, delimiter);
}

// Binary UNICODE Strings
inline bool LE_write_unicode_string(std::ostream& os, const char16* data, char16 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t elem = std::char_traits<char16>::length(data) + 1;
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 return !os.fail();
}

inline bool BE_write_unicode_string(std::ostream& os, const char16* data, char16 delimiter = 0)
{
 if(!data) return true; // nothing to write
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char16>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::unique_ptr<char16[]> copy(new char16[elem]);
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char16));
 return !os.fail();
}

inline bool LE_write_unicode_string(std::ostream& os, const char32* data, char32 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t elem = std::char_traits<char32>::length(data) + 1;
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 return !os.fail();
}

inline bool BE_write_unicode_string(std::ostream& os, const char32* data, char32 delimiter = 0)
{
 if(!data) return true; // nothing to write
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char32>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::unique_ptr<char32[]> copy(new char32[elem]);
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char32));
 return !os.fail();
}

#pragma endregion WRITING_FUNCTIONS

//
// READING AND WRITING FUNCTIONS
// ALTERNATE SYNTAX
//
#pragma region ALTERNATE_SYNTAX_FUNCTIONS

// Binary UNICODE Reading
inline char08 LE_read_char08(std::istream& is) { return LE_read<char08>(is); }
inline char08 BE_read_char08(std::istream& is) { return BE_read<char08>(is); }
inline char16 LE_read_char16(std::istream& is) { return LE_read<char16>(is); }
inline char16 BE_read_char16(std::istream& is) { return BE_read<char16>(is); }
inline char32 LE_read_char32(std::istream& is) { return LE_read<char32>(is); }
inline char32 BE_read_char32(std::istream& is) { return BE_read<char32>(is); }
inline char08 LE_read_char08(std::istream& is, std::streampos offset) { return LE_read<char08>(is, offset); }
inline char08 BE_read_char08(std::istream& is, std::streampos offset) { return BE_read<char08>(is, offset); }
inline char16 LE_read_char16(std::istream& is, std::streampos offset) { return LE_read<char16>(is, offset); }
inline char16 BE_read_char16(std::istream& is, std::streampos offset) { return BE_read<char16>(is, offset); }
inline char32 LE_read_char32(std::istream& is, std::streampos offset) { return LE_read<char32>(is, offset); }
inline char32 BE_read_char32(std::istream& is, std::streampos offset) { return BE_read<char32>(is, offset); }

// Binary UNICODE Writing
inline void LE_write_char08(std::ostream& os, char08 value) { LE_write(os, value); }
inline void BE_write_char08(std::ostream& os, char08 value) { BE_write(os, value); }
inline void LE_write_char16(std::ostream& os, char16 value) { LE_write(os, value); }
inline void BE_write_char16(std::ostream& os, char16 value) { BE_write(os, value); }
inline void LE_write_char32(std::ostream& os, char32 value) { LE_write(os, value); }
inline void BE_write_char32(std::ostream& os, char32 value) { BE_write(os, value); }

// Binary Integer Reading
inline sint08 LE_read_sint08(std::istream& is) { return LE_read<sint08>(is); }
inline sint08 BE_read_sint08(std::istream& is) { return BE_read<sint08>(is); }
inline uint08 LE_read_uint08(std::istream& is) { return LE_read<uint08>(is); }
inline uint08 BE_read_uint08(std::istream& is) { return BE_read<uint08>(is); }
inline sint16 LE_read_sint16(std::istream& is) { return LE_read<sint16>(is); }
inline sint16 BE_read_sint16(std::istream& is) { return BE_read<sint16>(is); }
inline uint16 LE_read_uint16(std::istream& is) { return LE_read<uint16>(is); }
inline uint16 BE_read_uint16(std::istream& is) { return BE_read<uint16>(is); }
inline sint32 LE_read_sint32(std::istream& is) { return LE_read<sint32>(is); }
inline sint32 BE_read_sint32(std::istream& is) { return BE_read<sint32>(is); }
inline uint32 LE_read_uint32(std::istream& is) { return LE_read<uint32>(is); }
inline uint32 BE_read_uint32(std::istream& is) { return BE_read<uint32>(is); }
inline sint64 LE_read_sint64(std::istream& is) { return LE_read<sint64>(is); }
inline sint64 BE_read_sint64(std::istream& is) { return BE_read<sint64>(is); }
inline uint64 LE_read_uint64(std::istream& is) { return LE_read<uint64>(is); }
inline uint64 BE_read_uint64(std::istream& is) { return BE_read<uint64>(is); }
inline sint08 LE_read_sint08(std::istream& is, std::streampos offset) { return LE_read<sint08>(is, offset); }
inline sint08 BE_read_sint08(std::istream& is, std::streampos offset) { return BE_read<sint08>(is, offset); }
inline uint08 LE_read_uint08(std::istream& is, std::streampos offset) { return LE_read<uint08>(is, offset); }
inline uint08 BE_read_uint08(std::istream& is, std::streampos offset) { return BE_read<uint08>(is, offset); }
inline sint16 LE_read_sint16(std::istream& is, std::streampos offset) { return LE_read<sint16>(is, offset); }
inline sint16 BE_read_sint16(std::istream& is, std::streampos offset) { return BE_read<sint16>(is, offset); }
inline uint16 LE_read_uint16(std::istream& is, std::streampos offset) { return LE_read<uint16>(is, offset); }
inline uint16 BE_read_uint16(std::istream& is, std::streampos offset) { return BE_read<uint16>(is, offset); }
inline sint32 LE_read_sint32(std::istream& is, std::streampos offset) { return LE_read<sint32>(is, offset); }
inline sint32 BE_read_sint32(std::istream& is, std::streampos offset) { return BE_read<sint32>(is, offset); }
inline uint32 LE_read_uint32(std::istream& is, std::streampos offset) { return LE_read<uint32>(is, offset); }
inline uint32 BE_read_uint32(std::istream& is, std::streampos offset) { return BE_read<uint32>(is, offset); }
inline sint64 LE_read_sint64(std::istream& is, std::streampos offset) { return LE_read<sint64>(is, offset); }
inline sint64 BE_read_sint64(std::istream& is, std::streampos offset) { return BE_read<sint64>(is, offset); }
inline uint64 LE_read_uint64(std::istream& is, std::streampos offset) { return LE_read<uint64>(is, offset); }
inline uint64 BE_read_uint64(std::istream& is, std::streampos offset) { return BE_read<uint64>(is, offset); }

// Binary Integer Writing
inline void LE_write_sint08(std::ostream& os, sint08 value) { LE_write(os, value); }
inline void BE_write_sint08(std::ostream& os, sint08 value) { BE_write(os, value); }
inline void LE_write_uint08(std::ostream& os, uint08 value) { LE_write(os, value); }
inline void BE_write_uint08(std::ostream& os, uint08 value) { BE_write(os, value); }
inline void LE_write_sint16(std::ostream& os, sint16 value) { LE_write(os, value); }
inline void BE_write_sint16(std::ostream& os, sint16 value) { BE_write(os, value); }
inline void LE_write_uint16(std::ostream& os, uint16 value) { LE_write(os, value); }
inline void BE_write_uint16(std::ostream& os, uint16 value) { BE_write(os, value); }
inline void LE_write_sint32(std::ostream& os, sint32 value) { LE_write(os, value); }
inline void BE_write_sint32(std::ostream& os, sint32 value) { BE_write(os, value); }
inline void LE_write_uint32(std::ostream& os, uint32 value) { LE_write(os, value); }
inline void BE_write_uint32(std::ostream& os, uint32 value) { BE_write(os, value); }
inline void LE_write_sint64(std::ostream& os, sint64 value) { LE_write(os, value); }
inline void BE_write_sint64(std::ostream& os, sint64 value) { BE_write(os, value); }
inline void LE_write_uint64(std::ostream& os, uint64 value) { LE_write(os, value); }
inline void BE_write_uint64(std::ostream& os, uint64 value) { BE_write(os, value); }

// Binary Real Reading
inline real32 LE_read_real32(std::istream& is) { return LE_read<real32>(is); }
inline real32 BE_read_real32(std::istream& is) { return BE_read<real32>(is); }
inline real64 LE_read_real64(std::istream& is) { return LE_read<real64>(is); }
inline real64 BE_read_real64(std::istream& is) { return BE_read<real64>(is); }
inline real32 LE_read_real32(std::istream& is, std::streampos offset) { return LE_read<real32>(is, offset); }
inline real32 BE_read_real32(std::istream& is, std::streampos offset) { return BE_read<real32>(is, offset); }
inline real64 LE_read_real64(std::istream& is, std::streampos offset) { return LE_read<real64>(is, offset); }
inline real64 BE_read_real64(std::istream& is, std::streampos offset) { return BE_read<real64>(is, offset); }

// Binary Real Writing
inline void LE_write_real32(std::ostream& os, real32 value) { LE_write(os, value); }
inline void BE_write_real32(std::ostream& os, real32 value) { BE_write(os, value); }
inline void LE_write_real64(std::ostream& os, real64 value) { LE_write(os, value); }
inline void BE_write_real64(std::ostream& os, real64 value) { BE_write(os, value); }

#pragma endregion ALTERNATE_SYNTAX_FUNCTIONS

#endif

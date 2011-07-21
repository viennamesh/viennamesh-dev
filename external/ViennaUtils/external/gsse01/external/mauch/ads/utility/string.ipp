// -*- C++ -*-
// ----------------------------------------------------------------------------
// Copyright 2006-2007 Sean Mauch. All rights reserved.
// See stlib/license.txt for license information.
// ----------------------------------------------------------------------------

#if !defined(__ads_utility_string_ipp__)
#error This file is an implementation detail of string.
#endif

BEGIN_NAMESPACE_ADS


// Split the string.
template <typename StringOutputIterator>
inline
int
split(const std::string& string, const std::string& separator, 
      StringOutputIterator output) {
  typedef std::string::size_type SizeType;
  
  // Words are separated of the separator.
  int numberOfWords = 0;

  assert(separator.length() != 0);
  
  const SizeType Size = string.length();
  const SizeType SeparatorSize = separator.length();
  const SizeType NoPosition = std::string::npos;

  // Position in the string.
  SizeType i = 0;

  // Skip any leading separators.
  while (i < Size && i == string.find(separator, i)) {
    i += SeparatorSize;
  }
  
  // Loop until we have processed the whole string.
  while (i < Size) {
    // Get the substring.
    const SizeType j = string.find(separator, i);
    if (j == NoPosition) {
      *output = string.substr(i, Size);
    }
    else {
      *output = string.substr(i, j - i);
    }
    ++numberOfWords;
    ++output;
    i = j;
    // Skip the separator(s).
    while (i < Size && i == string.find(separator, i)) {
      i += SeparatorSize;
    }
  }
  return numberOfWords;
}


// Make a zero-padded numerical extension.  Useful for constructing file names.
inline
void
makeZeroPaddedExtension(const int n, int maximumNumber, std::string* ext) {
  std::ostringstream iss;
  int width = 1;
  while (maximumNumber / 10 != 0) {
    ++width;
    maximumNumber /= 10;
  }
  iss << std::setw(width) << std::setfill('0') << n;
  *ext = iss.str();
}

END_NAMESPACE_ADS

// End of file.

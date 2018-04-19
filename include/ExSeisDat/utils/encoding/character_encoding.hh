////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Functions to convert between EBCDIC and ASCII encoding.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH
#define EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH

///
/// @namespace exseis::utils::character_encoding
///
/// @brief Functions for managing and converting between character encodings.
///

namespace exseis {
namespace utils {
inline namespace character_encoding {


/// @brief Convert an EBCDIC encoded character to an ASCII encoded character.
///
/// Returns the ASCII SUB character if there is no ASCII equivalent.
///
/// @param[in] ebcdic_char An EBCDIC encoded character.
///
/// @return An ASCII encoded character.
///
unsigned char to_ASCII_from_EBCDIC(unsigned char ebcdic_char);


/// @brief Convert an ASCII encoded character to an EBCDIC encoded character.
///
/// Returns the EBCDIC SUB character if there is no EBCDIC equivalent.
///
/// @param[in] ascii_char An ASCII encoded character.
///
/// @return An EBCDIC encoded character.
///
unsigned char to_EBCDIC_from_ASCII(unsigned char ascii_char);


/// @brief Determine whether an input character is a printable ASCII character.
///
/// @param[in] ascii_char The character to test.
///
/// @return Whether \c ascii_char represents a printable ASCII character.
///
bool is_printable_ASCII(unsigned char ascii_char);


/// @brief Determine whether an input character is a printable EBCDIC character.
///
/// @param[in] ebcdic_char The character to test.
///
/// @return Whether \c ebcdic_char represents a printable EBCDIC character.
///
bool is_printable_EBCDIC(unsigned char ebcdic_char);


}  // namespace character_encoding
}  // namespace utils
}  // namespace exseis

#endif  // EXSEISDAT_UTILS_ENCODING_CHARACTER_ENCODING_HH

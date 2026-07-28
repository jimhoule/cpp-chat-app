#pragma once

/**
 * NOTES:
 *  - TCode must be an enum class with a OK member
 *  - `code` reports whether a domain rule refused the operation
 *  - `payload` reports what was found. On any non-OK code, `payload` is left empty
 */
template<typename TCode, typename TData>
struct ServiceResult
{
    TCode code = TCode::OK;
    TData data = {};
};

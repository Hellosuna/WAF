// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FLATSTRARR_H_
#define FLATBUFFERS_GENERATED_FLATSTRARR_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 22 &&
              FLATBUFFERS_VERSION_MINOR == 11 &&
              FLATBUFFERS_VERSION_REVISION == 23,
             "Non-compatible flatbuffers version included");

struct FlatStrArr;
struct FlatStrArrBuilder;

struct FlatStrArr FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef FlatStrArrBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_STR_ARR = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *str_arr() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_STR_ARR);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_STR_ARR) &&
           verifier.VerifyVector(str_arr()) &&
           verifier.VerifyVectorOfStrings(str_arr()) &&
           verifier.EndTable();
  }
};

struct FlatStrArrBuilder {
  typedef FlatStrArr Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_str_arr(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> str_arr) {
    fbb_.AddOffset(FlatStrArr::VT_STR_ARR, str_arr);
  }
  explicit FlatStrArrBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<FlatStrArr> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<FlatStrArr>(end);
    return o;
  }
};

inline flatbuffers::Offset<FlatStrArr> CreateFlatStrArr(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> str_arr = 0) {
  FlatStrArrBuilder builder_(_fbb);
  builder_.add_str_arr(str_arr);
  return builder_.Finish();
}

inline flatbuffers::Offset<FlatStrArr> CreateFlatStrArrDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *str_arr = nullptr) {
  auto str_arr__ = str_arr ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*str_arr) : 0;
  return CreateFlatStrArr(
      _fbb,
      str_arr__);
}

inline const FlatStrArr *GetFlatStrArr(const void *buf) {
  return flatbuffers::GetRoot<FlatStrArr>(buf);
}

inline const FlatStrArr *GetSizePrefixedFlatStrArr(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FlatStrArr>(buf);
}

inline bool VerifyFlatStrArrBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatStrArr>(nullptr);
}

inline bool VerifySizePrefixedFlatStrArrBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatStrArr>(nullptr);
}

inline void FinishFlatStrArrBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatStrArr> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedFlatStrArrBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FlatStrArr> root) {
  fbb.FinishSizePrefixed(root);
}

#endif  // FLATBUFFERS_GENERATED_FLATSTRARR_H_

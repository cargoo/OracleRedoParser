#ifndef LOGICAL_ELEMS_H
#define LOGICAL_ELEMS_H

#include <string>
#include <sstream>
#include <stdint.h>
#include <map>
#include <memory>
#include <list>
#include "util/dtypes.h"

namespace databus {

  class ChangeHeader;
  class SCN {
   public:
    SCN() noexcept : major_(0), minor_(0), subscn_(0), noffset_(0) {}
    SCN(Ushort maj, uint32_t minor, unsigned int subscn = 0,
        uint32_t noffset = 0) noexcept : major_(maj),
                                         minor_(minor),
                                         subscn_(subscn),
                                         noffset_(noffset) {}
    SCN(int i) noexcept : major_(0xffff),
                          minor_(0xffffffff),
                          subscn_(0xffffffff),
                          noffset_(0xffffffff) {}

    bool operator<(const SCN& other) const;
    bool operator==(const SCN& other) const {
      return noffset_ == other.noffset_ && minor_ == other.minor_ &&
             major_ == other.major_ && subscn_ == other.subscn_;
    }
    std::string toStr() const;
    // To store this data into Database
    std::string toString() const;
    bool empty() const {
      return minor_ == 0 && major_ == 0 && subscn_ == 0 && noffset_ == 0;
    }

    Ushort major_;
    uint32_t minor_;
    unsigned int subscn_;
    uint32_t noffset_;

    uint64_t toNum() const;
  };  // SCN

  class RecordBuf {
   public:
    RecordBuf(const SCN& scn, uint32_t len, uint32_t epoch, char* change_buf,
              size_t offset, uint32_t seq, Uchar vld, bool allop = false);

    std::list<ChangeHeader*> change_vectors;
    size_t offset() const { return offset_; }

    ~RecordBuf() { delete[] change_buffers_; }
    SCN& scn() { return scn_; }
    Ushort op() { return op_; };
    uint32_t epoch() const { return epoch_; }

   private:
    // allop:  if true, capatural all opocde. or else only captual valid op
    void initChangeVectors(bool allop);

   private:
    SCN scn_;
    uint32_t epoch_;
    uint32_t change_length_;
    Ushort op_;
    char* change_buffers_;
    size_t offset_;  // debug only
   public:
    uint32_t seq_;  // logfile.seq#
    Uchar vld_;

  };  // RecordBuf

  typedef std::shared_ptr<RecordBuf> RecordBufPtr;

  struct ColumnChange {
    ColumnChange(Ushort col_id, Ushort len, char* content)
        : col_id_(col_id), len_(len), content_(content) {}
    ~ColumnChange() { delete[] content_; }
    void dump();

    // start with 1
    Ushort col_id_;
    Ushort len_;
    char* content_;
  };
  typedef std::shared_ptr<ColumnChange> ColumnChangePtr;
  typedef std::list<ColumnChangePtr> Row;
}  // databus
#endif

// SPDX-License-Identifier: Apache-2.0
//===-- ssvm/common/ast/instruction.h - Inst classes definition------------===//
//
// Part of the SSVM Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction node class and the
/// derived instruction classes.
///
//===----------------------------------------------------------------------===//
#pragma once

#include "common/errcode.h"
#include "common/types.h"
#include "common/value.h"
#include "loader/filemgr.h"
#include "support/variant.h"

#include <memory>
#include <vector>

namespace SSVM {
namespace AST {

/// Type aliasing
class Instruction;
using InstrVec = std::vector<std::unique_ptr<Instruction>>;
using InstrIter = InstrVec::const_iterator;

/// Loader class of Instruction node.
class Instruction {
public:
  /// Instruction opcode enumeration class.
  enum class OpCode : uint8_t {
    /// Control instructions
    Unreachable = 0x00,
    Nop = 0x01,
    Block = 0x02,
    Loop = 0x03,
    If = 0x04,
    Else = 0x05,
    End = 0x0B,
    Br = 0x0C,
    Br_if = 0x0D,
    Br_table = 0x0E,
    Return = 0x0F,
    Call = 0x10,
    Call_indirect = 0x11,

    /// Parametric Instructions
    Drop = 0x1A,
    Select = 0x1B,

    /// Variable Instructions
    Local__get = 0x20,
    Local__set = 0x21,
    Local__tee = 0x22,
    Global__get = 0x23,
    Global__set = 0x24,

    /// Memory Instructions
    I32__load = 0x28,
    I64__load = 0x29,
    F32__load = 0x2A,
    F64__load = 0x2B,
    I32__load8_s = 0x2C,
    I32__load8_u = 0x2D,
    I32__load16_s = 0x2E,
    I32__load16_u = 0x2F,
    I64__load8_s = 0x30,
    I64__load8_u = 0x31,
    I64__load16_s = 0x32,
    I64__load16_u = 0x33,
    I64__load32_s = 0x34,
    I64__load32_u = 0x35,
    I32__store = 0x36,
    I64__store = 0x37,
    F32__store = 0x38,
    F64__store = 0x39,
    I32__store8 = 0x3A,
    I32__store16 = 0x3B,
    I64__store8 = 0x3C,
    I64__store16 = 0x3D,
    I64__store32 = 0x3E,
    Memory__size = 0x3F,
    Memory__grow = 0x40,

    /// Const numeric instructions
    I32__const = 0x41,
    I64__const = 0x42,
    F32__const = 0x43,
    F64__const = 0x44,

    /// Numeric instructions
    I32__eqz = 0x45,
    I32__eq = 0x46,
    I32__ne = 0x47,
    I32__lt_s = 0x48,
    I32__lt_u = 0x49,
    I32__gt_s = 0x4A,
    I32__gt_u = 0x4B,
    I32__le_s = 0x4C,
    I32__le_u = 0x4D,
    I32__ge_s = 0x4E,
    I32__ge_u = 0x4F,
    I64__eqz = 0x50,
    I64__eq = 0x51,
    I64__ne = 0x52,
    I64__lt_s = 0x53,
    I64__lt_u = 0x54,
    I64__gt_s = 0x55,
    I64__gt_u = 0x56,
    I64__le_s = 0x57,
    I64__le_u = 0x58,
    I64__ge_s = 0x59,
    I64__ge_u = 0x5A,
    F32__eq = 0x5B,
    F32__ne = 0x5C,
    F32__lt = 0x5D,
    F32__gt = 0x5E,
    F32__le = 0x5F,
    F32__ge = 0x60,
    F64__eq = 0x61,
    F64__ne = 0x62,
    F64__lt = 0x63,
    F64__gt = 0x64,
    F64__le = 0x65,
    F64__ge = 0x66,
    I32__clz = 0x67,
    I32__ctz = 0x68,
    I32__popcnt = 0x69,
    I32__add = 0x6A,
    I32__sub = 0x6B,
    I32__mul = 0x6C,
    I32__div_s = 0x6D,
    I32__div_u = 0x6E,
    I32__rem_s = 0x6F,
    I32__rem_u = 0x70,
    I32__and = 0x71,
    I32__or = 0x72,
    I32__xor = 0x73,
    I32__shl = 0x74,
    I32__shr_s = 0x75,
    I32__shr_u = 0x76,
    I32__rotl = 0x77,
    I32__rotr = 0x78,
    I64__clz = 0x79,
    I64__ctz = 0x7a,
    I64__popcnt = 0x7b,
    I64__add = 0x7c,
    I64__sub = 0x7d,
    I64__mul = 0x7e,
    I64__div_s = 0x7f,
    I64__div_u = 0x80,
    I64__rem_s = 0x81,
    I64__rem_u = 0x82,
    I64__and = 0x83,
    I64__or = 0x84,
    I64__xor = 0x85,
    I64__shl = 0x86,
    I64__shr_s = 0x87,
    I64__shr_u = 0x88,
    I64__rotl = 0x89,
    I64__rotr = 0x8A,
    F32__abs = 0x8B,
    F32__neg = 0x8C,
    F32__ceil = 0x8D,
    F32__floor = 0x8E,
    F32__trunc = 0x8F,
    F32__nearest = 0x90,
    F32__sqrt = 0x91,
    F32__add = 0x92,
    F32__sub = 0x93,
    F32__mul = 0x94,
    F32__div = 0x95,
    F32__min = 0x96,
    F32__max = 0x97,
    F32__copysign = 0x98,
    F64__abs = 0x99,
    F64__neg = 0x9A,
    F64__ceil = 0x9B,
    F64__floor = 0x9C,
    F64__trunc = 0x9D,
    F64__nearest = 0x9E,
    F64__sqrt = 0x9F,
    F64__add = 0xA0,
    F64__sub = 0xA1,
    F64__mul = 0xA2,
    F64__div = 0xA3,
    F64__min = 0xA4,
    F64__max = 0xA5,
    F64__copysign = 0xA6,
    I32__wrap_i64 = 0xA7,
    I32__trunc_f32_s = 0xA8,
    I32__trunc_f32_u = 0xA9,
    I32__trunc_f64_s = 0xAA,
    I32__trunc_f64_u = 0xAB,
    I64__extend_i32_s = 0xAC,
    I64__extend_i32_u = 0xAD,
    I64__trunc_f32_s = 0xAE,
    I64__trunc_f32_u = 0xAF,
    I64__trunc_f64_s = 0xB0,
    I64__trunc_f64_u = 0xB1,
    F32__convert_i32_s = 0xB2,
    F32__convert_i32_u = 0xB3,
    F32__convert_i64_s = 0xB4,
    F32__convert_i64_u = 0xB5,
    F32__demote_f64 = 0xB6,
    F64__convert_i32_s = 0xB7,
    F64__convert_i32_u = 0xB8,
    F64__convert_i64_s = 0xB9,
    F64__convert_i64_u = 0xBA,
    F64__promote_f32 = 0xBB,
    I32__reinterpret_f32 = 0xBC,
    I64__reinterpret_f64 = 0xBD,
    F32__reinterpret_i32 = 0xBE,
    F64__reinterpret_i64 = 0xBF
  };

  /// Constructor assigns the OpCode.
  Instruction(const OpCode &Byte) : Code(Byte) {}
  virtual ~Instruction() noexcept = default;

  /// Binary loading from file manager. Default not load anything.
  virtual Expect<void> loadBinary(FileMgr &Mgr) { return {}; }

  /// Getter of OpCode.
  OpCode getOpCode() const { return Code; }

protected:
  /// OpCode if this instruction node.
  const OpCode Code;
};

/// Derived control instruction node.
class ControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  ControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  ControlInstruction(const ControlInstruction &Instr)
      : Instruction(Instr.Code) {}
};

/// Derived block control instruction node.
class BlockControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  BlockControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  BlockControlInstruction(const BlockControlInstruction &Instr);

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the return type, instructions in block body.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of block type
  ValType getResultType() const { return BlockType; }

  /// Getter of Block Body
  const InstrVec &getBody() const { return Body; }

private:
  /// \name Data of block instruction: return type and block body.
  /// @{
  ValType BlockType;
  InstrVec Body;
  /// @}
}; // namespace AST

/// Derived if-else control instruction node.
class IfElseControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  IfElseControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  IfElseControlInstruction(const IfElseControlInstruction &Instr);

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the return type, instructions in If and Else statements.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of block type
  ValType getResultType() const { return BlockType; }

  /// Getter of if statement.
  const InstrVec &getIfStatement() const { return IfStatement; }

  /// Getter of else statement.
  const InstrVec &getElseStatement() const { return ElseStatement; }

private:
  /// \name Data of block instruction: return type and statements.
  /// @{
  ValType BlockType;
  InstrVec IfStatement;
  InstrVec ElseStatement;
  /// @}
};

/// Derived branch control instruction node.
class BrControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  BrControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  BrControlInstruction(const BrControlInstruction &Instr)
      : Instruction(Instr.Code), LabelIdx(Instr.LabelIdx) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the branch label index.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Get label index
  uint32_t getLabelIndex() const { return LabelIdx; }

private:
  /// Branch-to label index.
  uint32_t LabelIdx = 0;
};

/// Derived branch table control instruction node.
class BrTableControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  BrTableControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  BrTableControlInstruction(const BrTableControlInstruction &Instr)
      : Instruction(Instr.Code), LabelTable(Instr.LabelTable),
        LabelIdx(Instr.LabelIdx) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the vector of labels and default branch label of indirect branch.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of label table
  const std::vector<uint32_t> &getLabelTable() const { return LabelTable; }

  /// Getter of label index
  uint32_t getLabelIndex() const { return LabelIdx; }

private:
  /// \name Data of branch instruction: label vector and defalt label.
  /// @{
  std::vector<uint32_t> LabelTable;
  uint32_t LabelIdx = 0;
  /// @}
};

/// Derived call control instruction node.
class CallControlInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  CallControlInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  CallControlInstruction(const CallControlInstruction &Instr)
      : Instruction(Instr.Code), FuncIdx(Instr.FuncIdx) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the function index.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of the index
  uint32_t getFuncIndex() const { return FuncIdx; }

private:
  /// Call function index.
  uint32_t FuncIdx = 0;
};

/// Derived parametric instruction node.
class ParametricInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  ParametricInstruction(const OpCode &Byte) : Instruction(Byte) {}
};

/// Derived variable instruction node.
class VariableInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  VariableInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  VariableInstruction(const VariableInstruction &Instr)
      : Instruction(Instr.Code), VarIdx(Instr.VarIdx) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the global or local variable index.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of the index
  uint32_t getVariableIndex() const { return VarIdx; }

private:
  /// Global or local index.
  uint32_t VarIdx = 0;
};

/// Derived memory instruction node.
class MemoryInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  MemoryInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  MemoryInstruction(const MemoryInstruction &Instr)
      : Instruction(Instr.Code), Align(Instr.Align), Offset(Instr.Offset) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read the memory arguments: alignment and offset.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getters of memory align and offset.
  uint32_t getMemoryAlign() const { return Align; }
  uint32_t getMemoryOffset() const { return Offset; }

private:
  /// \name Data of memory instruction: Alignment and offset.
  /// @{
  uint32_t Align = 0;
  uint32_t Offset = 0;
  /// @}
};

/// Derived const numeric instruction node.
class ConstInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  ConstInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  ConstInstruction(const ConstInstruction &Instr)
      : Instruction(Instr.Code), Num(Instr.Num) {}

  /// Load binary from file manager.
  ///
  /// Inheritted and overrided from Instruction.
  /// Read and decode the const value.
  ///
  /// \param Mgr the file manager reference.
  ///
  /// \returns void when success, ErrMsg when failed.
  Expect<void> loadBinary(FileMgr &Mgr) override;

  /// Getter of the constant value.
  ValVariant getConstValue() const { return Num; }

private:
  /// Const value of this instruction.
  ValVariant Num;
};

/// Derived numeric instruction node.
class UnaryNumericInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  UnaryNumericInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  UnaryNumericInstruction(const UnaryNumericInstruction &Instr)
      : Instruction(Instr.Code) {}
};

/// Derived numeric instruction node.
class BinaryNumericInstruction : public Instruction {
public:
  /// Call base constructor to initialize OpCode.
  BinaryNumericInstruction(const OpCode &Byte) : Instruction(Byte) {}
  /// Copy constructor.
  BinaryNumericInstruction(const BinaryNumericInstruction &Instr)
      : Instruction(Instr.Code) {}
};

template <typename T>
auto dispatchInstruction(Instruction::OpCode Code, T &&Visitor) {
  switch (Code) {
    /// The OpCode::End and OpCode::Else will not make nodes.
  case Instruction::OpCode::Unreachable:
  case Instruction::OpCode::Nop:
  case Instruction::OpCode::Return:
    return Visitor(Support::tag<ControlInstruction>());

  case Instruction::OpCode::Block:
  case Instruction::OpCode::Loop:
    return Visitor(Support::tag<BlockControlInstruction>());

  case Instruction::OpCode::If:
    return Visitor(Support::tag<IfElseControlInstruction>());

  case Instruction::OpCode::Br:
  case Instruction::OpCode::Br_if:
    return Visitor(Support::tag<BrControlInstruction>());

  case Instruction::OpCode::Br_table:
    return Visitor(Support::tag<BrTableControlInstruction>());

  case Instruction::OpCode::Call:
  case Instruction::OpCode::Call_indirect:
    return Visitor(Support::tag<CallControlInstruction>());

  case Instruction::OpCode::Drop:
  case Instruction::OpCode::Select:
    return Visitor(Support::tag<ParametricInstruction>());

  case Instruction::OpCode::Local__get:
  case Instruction::OpCode::Local__set:
  case Instruction::OpCode::Local__tee:
  case Instruction::OpCode::Global__get:
  case Instruction::OpCode::Global__set:
    return Visitor(Support::tag<VariableInstruction>());

  case Instruction::OpCode::I32__load:
  case Instruction::OpCode::I64__load:
  case Instruction::OpCode::F32__load:
  case Instruction::OpCode::F64__load:
  case Instruction::OpCode::I32__load8_s:
  case Instruction::OpCode::I32__load8_u:
  case Instruction::OpCode::I32__load16_s:
  case Instruction::OpCode::I32__load16_u:
  case Instruction::OpCode::I64__load8_s:
  case Instruction::OpCode::I64__load8_u:
  case Instruction::OpCode::I64__load16_s:
  case Instruction::OpCode::I64__load16_u:
  case Instruction::OpCode::I64__load32_s:
  case Instruction::OpCode::I64__load32_u:
  case Instruction::OpCode::I32__store:
  case Instruction::OpCode::I64__store:
  case Instruction::OpCode::F32__store:
  case Instruction::OpCode::F64__store:
  case Instruction::OpCode::I32__store8:
  case Instruction::OpCode::I32__store16:
  case Instruction::OpCode::I64__store8:
  case Instruction::OpCode::I64__store16:
  case Instruction::OpCode::I64__store32:
  case Instruction::OpCode::Memory__size:
  case Instruction::OpCode::Memory__grow:
    return Visitor(Support::tag<MemoryInstruction>());

  case Instruction::OpCode::I32__const:
  case Instruction::OpCode::I64__const:
  case Instruction::OpCode::F32__const:
  case Instruction::OpCode::F64__const:
    return Visitor(Support::tag<ConstInstruction>());

  case Instruction::OpCode::I32__eqz:
  case Instruction::OpCode::I32__clz:
  case Instruction::OpCode::I32__ctz:
  case Instruction::OpCode::I32__popcnt:
  case Instruction::OpCode::I64__eqz:
  case Instruction::OpCode::I64__clz:
  case Instruction::OpCode::I64__ctz:
  case Instruction::OpCode::I64__popcnt:
  case Instruction::OpCode::F32__abs:
  case Instruction::OpCode::F32__neg:
  case Instruction::OpCode::F32__ceil:
  case Instruction::OpCode::F32__floor:
  case Instruction::OpCode::F32__trunc:
  case Instruction::OpCode::F32__nearest:
  case Instruction::OpCode::F32__sqrt:
  case Instruction::OpCode::F64__abs:
  case Instruction::OpCode::F64__neg:
  case Instruction::OpCode::F64__ceil:
  case Instruction::OpCode::F64__floor:
  case Instruction::OpCode::F64__trunc:
  case Instruction::OpCode::F64__nearest:
  case Instruction::OpCode::F64__sqrt:
  case Instruction::OpCode::I32__wrap_i64:
  case Instruction::OpCode::I32__trunc_f32_s:
  case Instruction::OpCode::I32__trunc_f32_u:
  case Instruction::OpCode::I32__trunc_f64_s:
  case Instruction::OpCode::I32__trunc_f64_u:
  case Instruction::OpCode::I64__extend_i32_s:
  case Instruction::OpCode::I64__extend_i32_u:
  case Instruction::OpCode::I64__trunc_f32_s:
  case Instruction::OpCode::I64__trunc_f32_u:
  case Instruction::OpCode::I64__trunc_f64_s:
  case Instruction::OpCode::I64__trunc_f64_u:
  case Instruction::OpCode::F32__convert_i32_s:
  case Instruction::OpCode::F32__convert_i32_u:
  case Instruction::OpCode::F32__convert_i64_s:
  case Instruction::OpCode::F32__convert_i64_u:
  case Instruction::OpCode::F32__demote_f64:
  case Instruction::OpCode::F64__convert_i32_s:
  case Instruction::OpCode::F64__convert_i32_u:
  case Instruction::OpCode::F64__convert_i64_s:
  case Instruction::OpCode::F64__convert_i64_u:
  case Instruction::OpCode::F64__promote_f32:
  case Instruction::OpCode::I32__reinterpret_f32:
  case Instruction::OpCode::I64__reinterpret_f64:
  case Instruction::OpCode::F32__reinterpret_i32:
  case Instruction::OpCode::F64__reinterpret_i64:
    return Visitor(Support::tag<UnaryNumericInstruction>());

  case Instruction::OpCode::I32__eq:
  case Instruction::OpCode::I32__ne:
  case Instruction::OpCode::I32__lt_s:
  case Instruction::OpCode::I32__lt_u:
  case Instruction::OpCode::I32__gt_s:
  case Instruction::OpCode::I32__gt_u:
  case Instruction::OpCode::I32__le_s:
  case Instruction::OpCode::I32__le_u:
  case Instruction::OpCode::I32__ge_s:
  case Instruction::OpCode::I32__ge_u:
  case Instruction::OpCode::I64__eq:
  case Instruction::OpCode::I64__ne:
  case Instruction::OpCode::I64__lt_s:
  case Instruction::OpCode::I64__lt_u:
  case Instruction::OpCode::I64__gt_s:
  case Instruction::OpCode::I64__gt_u:
  case Instruction::OpCode::I64__le_s:
  case Instruction::OpCode::I64__le_u:
  case Instruction::OpCode::I64__ge_s:
  case Instruction::OpCode::I64__ge_u:
  case Instruction::OpCode::F32__eq:
  case Instruction::OpCode::F32__ne:
  case Instruction::OpCode::F32__lt:
  case Instruction::OpCode::F32__gt:
  case Instruction::OpCode::F32__le:
  case Instruction::OpCode::F32__ge:
  case Instruction::OpCode::F64__eq:
  case Instruction::OpCode::F64__ne:
  case Instruction::OpCode::F64__lt:
  case Instruction::OpCode::F64__gt:
  case Instruction::OpCode::F64__le:
  case Instruction::OpCode::F64__ge:

  case Instruction::OpCode::I32__add:
  case Instruction::OpCode::I32__sub:
  case Instruction::OpCode::I32__mul:
  case Instruction::OpCode::I32__div_s:
  case Instruction::OpCode::I32__div_u:
  case Instruction::OpCode::I32__rem_s:
  case Instruction::OpCode::I32__rem_u:
  case Instruction::OpCode::I32__and:
  case Instruction::OpCode::I32__or:
  case Instruction::OpCode::I32__xor:
  case Instruction::OpCode::I32__shl:
  case Instruction::OpCode::I32__shr_s:
  case Instruction::OpCode::I32__shr_u:
  case Instruction::OpCode::I32__rotl:
  case Instruction::OpCode::I32__rotr:
  case Instruction::OpCode::I64__add:
  case Instruction::OpCode::I64__sub:
  case Instruction::OpCode::I64__mul:
  case Instruction::OpCode::I64__div_s:
  case Instruction::OpCode::I64__div_u:
  case Instruction::OpCode::I64__rem_s:
  case Instruction::OpCode::I64__rem_u:
  case Instruction::OpCode::I64__and:
  case Instruction::OpCode::I64__or:
  case Instruction::OpCode::I64__xor:
  case Instruction::OpCode::I64__shl:
  case Instruction::OpCode::I64__shr_s:
  case Instruction::OpCode::I64__shr_u:
  case Instruction::OpCode::I64__rotl:
  case Instruction::OpCode::I64__rotr:
  case Instruction::OpCode::F32__add:
  case Instruction::OpCode::F32__sub:
  case Instruction::OpCode::F32__mul:
  case Instruction::OpCode::F32__div:
  case Instruction::OpCode::F32__min:
  case Instruction::OpCode::F32__max:
  case Instruction::OpCode::F32__copysign:
  case Instruction::OpCode::F64__add:
  case Instruction::OpCode::F64__sub:
  case Instruction::OpCode::F64__mul:
  case Instruction::OpCode::F64__div:
  case Instruction::OpCode::F64__min:
  case Instruction::OpCode::F64__max:
  case Instruction::OpCode::F64__copysign:
    return Visitor(Support::tag<BinaryNumericInstruction>());

  default:
    return Visitor(Support::tag<void>());
  }
}

/// Make the new instruction node.
///
/// Select the node type corresponding to the input Code.
/// Create the derived instruction class and return pointer.
///
/// \param Code the OpCode of instruction to make.
///
/// \returns unique pointer of instruction node if success, ErrMsg when failed.
Expect<std::unique_ptr<Instruction>>
makeInstructionNode(const Instruction::OpCode &Code);

/// Make the new instruction node from old one.
///
/// Select the node type corresponding to the input Code.
/// Create the duplicated instruction node and return pointer.
///
/// \param Instr the instruction to duplicate.
///
/// \returns unique pointer of instruction node if success, ErrMsg when failed.
Expect<std::unique_ptr<Instruction>>
makeInstructionNode(const Instruction &Instr);

} // namespace AST
} // namespace SSVM

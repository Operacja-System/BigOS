/*
Language: RISC-V Assembly
Author: aana_h2 <aana_h2@outlook.com>
Description: RISC-V64GC assembly language
Website: https://riscv.org/technical/specifications/
Category: assembler
*/

const RISCV_REGISTERS = [
  'x0', 'x1', 'x2', 'x3', 'x4', 'x5', 'x6', 'x7',
  'x8', 'x9', 'x10', 'x11', 'x12', 'x13', 'x14', 'x15',
  'x16', 'x17', 'x18', 'x19', 'x20', 'x21', 'x22', 'x23',
  'x24', 'x25', 'x26', 'x27', 'x28', 'x29', 'x30', 'x31', 'pc',
  'f0', 'f1', 'f2', 'f3', 'f4', 'f5', 'f6', 'f7',
  'f8', 'f9', 'f10', 'f11', 'f12', 'f13', 'f14', 'f15',
  'f16', 'f17', 'f18', 'f19', 'f20', 'f21', 'f22', 'f23',
  'f24', 'f25', 'f26', 'f27', 'f28', 'f29', 'f30', 'f31',
  'zero', 'ra', 'sp', 'gp', 'tp', 't0', 't1', 't2', 's0',
  's1', 'a0', 'a1', 'a2', 'a3', 'a4', 'a5', 'a6', 'a7', 's2',
  's3', 's4', 's5', 's6', 's7', 's8', 's9', 's10', 's11', 't3',
  't4', 't5', 't6', 'fp'
];

// directive
const RISCV_DIRECTIVES = [
  '.align', '.file', '.globl', '.local', '.comm', '.common', '.ident',
  '.section', '.size', '.text', '.data', '.rodata', '.bss', '.string',
  '.asciz', '.equ', '.marco', '.endm', '.type', '.option', '.2byte',
  '.half', '.short', '.4byte', '.word', '.long', '.8byte', '.dword',
  '.quad', '.float', '.double', '.dtprelword', '.dtpreldword', '.sleb128',
  '.uleb128', '.p2align', '.balign', '.zero', '.variant_cc', '.attribute'
];

const RISCV_MNEMONICS = [
  'lui', 'auipc', 'jal', 'jalr', 'beq', 'bne', 'blt', 'bge', 'bltu',
  'bgeu', 'lb', 'lh', 'lw', 'lbu', 'lhu', 'sb', 'sh', 'sw', 'addi',
  'slti', 'sltiu', 'xori', 'ori', 'andi', 'slli', 'srli', 'srai', 'add',
  'sub', 'sll', 'slt', 'sltu', 'xor', 'srl', 'sra', 'or', 'and', 'fence',
  'ecall', 'ebreak', // RV32I
  'lwu', 'ld', 'sd', 'addiw', 'slliw', 'srliw', 'sraiw', 'addw', 'subw',
  'sllw', 'srlw', 'sraw', // RV64I
  'mul', 'mulh', 'mulhsu', 'mulhu', 'div', 'divu', 'rem', 'remu', // RV32M
  'mulw', 'divw', 'divuw', 'remw', 'remuw', // RV64M
  'flw', 'fsw', 'fmadd.s', 'fmsub.s', 'fnmsub.s', 'fnmadd.s', 'fadd.s', 'fsub.s', 'fmul.s',
  'fdiv.s', 'fsqrt.s', 'fsgnj.s', 'fsgnjn.s', 'fsgnjx.s', 'fmin.s', 'fmax.s', 'fcvt.w.s',
  'fcvt.wu.s', 'fmv.x.w', 'feq.s', 'flt.s', 'fle.s', 'fclass.s', 'fcvt.s.w', 'fcvt.s.wu',
  'fmv.w.x', // RV32F
  'fcvt.l.s', 'fcvt.lu.s', 'fcvt.s.l', 'fcvt.s.lu', // RV64F
  'fld', 'fsd', 'fmadd.d', 'fmsub.d', 'fnmsub.d', 'fnmadd.d', 'fadd.d', 'fsub.d', 'fmul.d',
  'fdiv.d', 'fsqrt.d', 'fsgnj.d', 'fsgnjn.d', 'fsgnjx.d', 'fmin.d', 'fmax.d', 'fcvt.s.d',
  'fcvt.d.s', 'feq.d', 'flt.d', 'fle.d', 'fclass.d', 'fcvt.w.d', 'fcvt.wu.d', 'fcvt.d.w',
  'fcvt.d.wu', // RV32D
  'fcvt.l.d', 'fcvt.lu.d', 'fmv.x.d', 'fcvt.d.l', 'fcvt.d.lu', 'fmv.d.x', // RV64D
  'lr.w', 'sc.w', 'amoswap.w', 'amoadd.w', 'amoxor.w', 'amoand.w', 'amoor.w', 'amomin.w',
  'amomax.w', 'amominu.w', 'amomaxu.w', // RV32A
  'lr.d', 'sc.d', 'amoswap.d', 'amoadd.d', 'amoxor.d', 'amoand.d', 'amoor.d', 'amomin.d',
  'amomax.d', 'amominu.d', 'amomaxu.d', // RV64A
  'csrrw', 'csrrs', 'csrrc', 'csrrwi', 'csrrsi', 'csrrci', // Zicsr
  'fence.i', // Zifencei
];

const RISCV_PSEUDOS = [
  'la', 'lla',
  'nop', 'li', 'mv', 'not', 'neg', 'negw', 'sext.w', 'seqz', 'snez', 'sltz', 'sgtz',
  'fmv.s', 'fabs.s', 'fneg.s', 'fmv.d', 'fabs.d', 'fneg.d',
  'beqz', 'bnez', 'blez', 'bgez', 'bltz', 'bgtz',
  'bgt', 'ble', 'bgtu', 'bleu',
  'j', 'jal', 'jr', 'jalr', 'ret', 'call', 'tail',
  'fence',
  'rdinstret', 'rdcycle', 'rdtime',
  'csrr', 'csrw', 'csrs', 'csrc', 'csrwi', 'csrsi', 'csrci', 'frcsr', 'fscsr', 'fscsr',
  'frrm', 'fsrm', 'fsrm', 'frflags', 'fsflags', 'fsflags'
];

const RISCV_PRIVILEGED = [
  'sret', 'mret', 'wfi', 'sfence.vma', 'sinval.vma', 'sfence.w.inval', 'sfence.inval.ir',
  'hfence.vvma', 'hfence.gvma', 'hinval.vvma', 'hinval.gvma', 'hlv.b', 'hlv.bu', 'hlv.h',
  'hlv.hu', 'hlvx.hu', 'hlv.w', 'hlvx.wu', 'hsv.b', 'hsv.h', 'hsv.w', 'hlv.wu', 'hlv.d',
  'hsv.d'
];

const RISCV_KEYWORD = RISCV_MNEMONICS.concat(RISCV_PSEUDOS).concat(RISCV_PRIVILEGED);

hljs.registerLanguage('riscv', function(hljs) {
  return {
    name: 'RISC-V Assembly',
    aliases: ['riscv', 'riscv-asm'],
    case_insensitive: true,
    keywords: {
      keyword: RISCV_KEYWORD.join(' '),
      meta: RISCV_DIRECTIVES.join(' '),
      built_in: RISCV_REGISTERS.join(' ')
    },
    contains: [
      hljs.COMMENT('#', '$'),
      hljs.C_LINE_COMMENT_MODE,
      hljs.C_BLOCK_COMMENT_MODE,
      {
        scope: 'number',
        variants: [
          { // hex
            begin: '0x[0-9a-f]+'
          },
          { // bin
            begin: '0b[01]+'
          },
          { // dec
            begin: '(?<![\\w.])-?(0|[1-9]\\d*)(?![:\\w])'
          },
          { // float
            begin: '\\b-?\\d+\\.\\d+'
          }
        ],
        relevance: 0
      },
      {
        className: 'symbol',
        variants: [
          // Global label and local label
          { begin: '^\\s*[A-Za-z._?][A-Za-z0-9_$#@~.?]*(:|\\s+label)' },
          // Macro-local label
          { begin: '^\\s*%%[A-Za-z0-9_$#@~.?]*:' },
          // numbered local label
          { begin: '^\\s*[0-9]+:' },
          // numbered local label reference
          { begin: '[0-9]+[bf](?!\\w)' },
        ],
        relevance: 0
      }
    ]
  };
});

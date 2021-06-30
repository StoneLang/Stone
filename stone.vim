" Vim syntax file
" Language:     Stone
" Maintainer:   
" For bugs, patches and license go to https://github.com/stonelang/stone/stone.vim

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

" Syntax definitions {{{1
" Basic keywords {{{2
syn keyword   stoneConditional match if else switch
syn keyword   stoneRepeat for loop while do
syn keyword   stoneTypedef type nextgroup=stoneIdentifier skipwhite skipempty
syn keyword   stoneStructure struct enum interface nextgroup=stoneIdentifier skipwhite skipempty

syn keyword   stoneUnion union nextgroup=stoneIdentifier skipwhite skipempty contained
syn match stoneUnionContextual /\<union\_s\+\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*/ transparent contains=stoneUnion
syn keyword   stoneOperator    as

syn match     stoneAssert      "\<assert\(\w\)*!" contained
syn match     stonePanic       "\<panic\(\w\)*!" contained
syn keyword   stoneKeyword     break 
syn keyword   stoneKeyword     continue new property free self space module import with
syn keyword   stoneKeyword     extern nextgroup=stoneExternCrate,stoneObsoleteExternMod skipwhite skipempty
syn keyword   stoneKeyword     fun nextgroup=stoneFunctionName skipwhite skipempty
syn keyword   stoneKeyword     prop nextgroup=stonePropType skipwhite skipempty

syn keyword   stoneKeyword     in is this private protected public pin safe 
syn keyword		stoneKeyword		 auto any  final free  
syn keyword   stoneKeyword     public  nextgroup=stonePubScope skipwhite skipempty
syn keyword   stoneKeyword     return void object using  null 
syn keyword   stoneSuper       super
syn keyword   stoneKeyword     where safe pint own stone get mutable inline case default
syn keyword   stoneKeyword     use nextgroup=stoneModPath skipwhite skipempty
" FIXME: Scoped impl's name is also fallen in this category
syn keyword   stoneKeyword     mod trait nextgroup=stoneIdentifier skipwhite skipempty
syn keyword   stoneStorage     move ref static const 
syn match stoneDefault /\<default\ze\_s\+\(impl\|fn\|type\|const\)\>/

syn keyword   stoneInvalidBareKeyword crate

syn keyword stonePubScopeCrate crate contained
syn match stonePubScopeDelim /[()]/ contained
syn match stonePubScope /([^()]*)/ contained contains=stonePubScopeDelim,stonePubScopeCrate,stoneSuper,stoneModPath,stoneModPathSep,stoneThis transparent

syn keyword   stoneExternCrate crate contained nextgroup=stoneIdentifier,stoneExternCrateString skipwhite skipempty
" This is to get the `bar` part of `extern crate "foo" as bar;` highlighting.
syn match   stoneExternCrateString /".*"\_s*as/ contained nextgroup=stoneIdentifier skipwhite transparent skipempty contains=stoneString,stoneOperator
syn keyword   stoneObsoleteExternMod mod contained nextgroup=stoneIdentifier skipwhite skipempty

syn match     stoneIdentifier  contains=stoneIdentifierPrime "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained
syn match     stoneFunctionName    "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained

syn region    stoneBoxPlacement matchgroup=stoneBoxPlacementParens start="(" end=")" contains=TOP contained
" Ideally we'd have syntax rules set up to match arbitrary expressions. Since
" we don't, we'll just define temporary contained rules to handle balancing
" delimiters.
syn region    stoneBoxPlacementBalance start="(" end=")" containedin=stoneBoxPlacement transparent
syn region    stoneBoxPlacementBalance start="\[" end="\]" containedin=stoneBoxPlacement transparent
" {} are handled by stoneFoldBraces

syn region stoneMacroRepeat matchgroup=stoneMacroRepeatDelimiters start="$(" end=")" contains=TOP nextgroup=stoneMacroRepeatCount
syn match stoneMacroRepeatCount ".\?[*+]" contained
syn match stoneMacroVariable "$\w\+"

" Reserved (but not yet used) keywords {{{2
syn keyword   stoneReservedKeyword alignof become offsetof priv sizeof typeof unsized yield abstract  macro

" Built-in types {{{2
syn keyword   stoneType        byte char bool uint8 uint16 uint32 uint64 uint128 float32 float64
syn keyword   stoneType        int8 int16 int32 int64 int128 string this int unit complex32 complex64 

" Things from the libstd v1 prelude (src/libstd/prelude/v1.rs) {{{2
" This section is just straight transformation of the contents of the prelude,
" to make it easy to update.

" Reexported core operators {{{3
syn keyword   stoneInterface      TODO 

" Reexported functions {{{3
" There’s no point in highlighting these; when one writes drop( or drop::< it
" gets the same highlighting anyway, and if someone writes `let drop = …;` we
" don’t really want *that* drop to be highlighted.
"syn keyword stoneFunction drop

" Reexported types and traits {{{3
"syn keyword stoneInterface Box
"syn keyword stoneInterface ToOwned
"syn keyword stoneInterface Clone
"syn keyword stoneInterface PartialEq PartialOrd Eq Ord
"syn keyword stoneInterface AsRef AsMut Into From
"syn keyword stoneInterface Default
"syn keyword stoneInterface Iterator Extend IntoIterator
"syn keyword stoneInterface DoubleEndedIterator ExactSizeIterator
"syn keyword stoneEnum Option
"syn keyword stoneEnumVariant Some None
"syn keyword stoneEnum Result
"syn keyword stoneEnumVariant Ok Err
"syn keyword stoneInterface SliceConcatExt
"syn keyword stoneInterface String ToString
"syn keyword stoneInterface Vec

" Other syntax {{{2
syn keyword   stoneThis        this
syn keyword   stoneBoolean     true false

" If foo::bar changes to foo.bar, change this ("::" to "\.").
" If foo::bar changes to Foo::bar, change this (first "\w" to "\u").
syn match     stoneModPath     "\w\(\w\)*::[^<]"he=e-3,me=e-3
syn match     stoneModPathSep  "::"

syn match     stoneFunCall    "\w\(\w\)*("he=e-1,me=e-1
syn match     stoneFunCall    "\w\(\w\)*::<"he=e-3,me=e-3 " foo::<T>();

" This is merely a convention; note also the use of [A-Z], restricting it to
" latin identifiers rather than the full Unicode uppercase. I have not used
" [:upper:] as it depends upon 'noignorecase'
"syn match     stoneCapsIdent    display "[A-Z]\w\(\w\)*"

syn match     stoneOperator     display "\%(+\|-\|/\|*\|=\|\^\|&\||\|!\|>\|<\|%\)=\?"
" This one isn't *quite* right, as we could have binary-& with a reference
syn match     stoneSigil        display /&\s\+[&~@*][^)= \t\r\n]/he=e-1,me=e-1
syn match     stoneSigil        display /[&~@*][^)= \t\r\n]/he=e-1,me=e-1
" This isn't actually correct; a closure with no arguments can be `|| { }`.
" Last, because the & in && isn't a sigil
syn match     stoneOperator     display "&&\|||"
" This is stoneArrowCharacter rather than stoneArrow for the sake of matchparen,
" so it skips the ->; see http://stackoverflow.com/a/30309949 for details.
syn match     stoneArrowCharacter display "->"
syn match     stoneQuestionMark display "?\([a-zA-Z]\+\)\@!"

syn match     stoneMacro       '\w\(\w\)*!' contains=stoneAssert,stonePanic
syn match     stoneMacro       '#\w\(\w\)*' contains=stoneAssert,stonePanic

syn match     stoneEscapeError   display contained /\\./
syn match     stoneEscape        display contained /\\\([nrt0\\'"]\|x\x\{2}\)/
syn match     stoneEscapeUnicode display contained /\\u{\x\{1,6}}/
syn match     stoneStringContinuation display contained /\\\n\s*/
syn region    stoneString      start=+b"+ skip=+\\\\\|\\"+ end=+"+ contains=stoneEscape,stoneEscapeError,stoneStringContinuation
syn region    stoneString      start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=stoneEscape,stoneEscapeUnicode,stoneEscapeError,stoneStringContinuation,@Spell
syn region    stoneString      start='b\?r\z(#*\)"' end='"\z1' contains=@Spell

syn region    stoneAttribute   start="#!\?\[" end="\]" contains=stoneString,stoneDerive,stoneCommentLine,stoneCommentBlock,stoneCommentLineDocError,stoneCommentBlockDocError
syn region    stoneDerive      start="derive(" end=")" contained contains=stoneDeriveInterface
" This list comes from src/libsyntax/ext/deriving/mod.rs
" Some are deprecated (Encodable, Decodable) or to be removed after a new snapshot (Show).
syn keyword   stoneDeriveInterface contained Clone Hash RustcEncodable RustcDecodable Encodable Decodable PartialEq Eq PartialOrd Ord Rand Show Debug Default FromPrimitive Send Sync Copy

" Number literals
syn match     stoneDecNumber   display "\<[0-9][0-9_]*\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     stoneHexNumber   display "\<0x[a-fA-F0-9_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     stoneOctNumber   display "\<0o[0-7_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     stoneBinNumber   display "\<0b[01_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="

" Special case for numbers of the form "1." which are float literals, unless followed by
" an identifier, which makes them integer literals with a method call or field access,
" or by another ".", which makes them integer literals followed by the ".." token.
" (This must go first so the others take precedence.)
syn match     stoneFloat       display "\<[0-9][0-9_]*\.\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\|\.\)\@!"
" To mark a number as a normal float, it must have at least one of the three things integral values don't have:
" a decimal point and more numbers; an exponent; and a type suffix.
syn match     stoneFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)\="
syn match     stoneFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\(f32\|f64\)\="
syn match     stoneFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)"

" For the benefit of delimitMate
syn region stoneLifetimeCandidate display start=/&'\%(\([^'\\]\|\\\(['nrt0\\\"]\|x\x\{2}\|u{\x\{1,6}}\)\)'\)\@!/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=stoneSigil,stoneLifetime
syn region stoneGenericRegion display start=/<\%('\|[^[cntrl:][:space:][:punct:]]\)\@=')\S\@=/ end=/>/ contains=stoneGenericLifetimeCandidate
syn region stoneGenericLifetimeCandidate display start=/\%(<\|,\s*\)\@<='/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=stoneSigil,stoneLifetime

"stoneLifetime must appear before stoneCharacter, or chars will get the lifetime highlighting
syn match     stoneLifetime    display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*"
syn match     stoneLabel       display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*:"
syn match   stoneCharacterInvalid   display contained /b\?'\zs[\n\r\t']\ze'/
" The groups negated here add up to 0-255 but nothing else (they do not seem to go beyond ASCII).
syn match   stoneCharacterInvalidUnicode   display contained /b'\zs[^[:cntrl:][:graph:][:alnum:][:space:]]\ze'/
syn match   stoneCharacter   /b'\([^\\]\|\\\(.\|x\x\{2}\)\)'/ contains=stoneEscape,stoneEscapeError,stoneCharacterInvalid,stoneCharacterInvalidUnicode
syn match   stoneCharacter   /'\([^\\]\|\\\(.\|x\x\{2}\|u{\x\{1,6}}\)\)'/ contains=stoneEscape,stoneEscapeUnicode,stoneEscapeError,stoneCharacterInvalid

syn match stoneShebang /\%^#![^[].*/
syn region stoneCommentLine                                                  start="//"                      end="$"   contains=stoneTodo,@Spell
syn region stoneCommentLineDoc                                               start="//\%(//\@!\|!\)"         end="$"   contains=stoneTodo,@Spell
syn region stoneCommentLineDocError                                          start="//\%(//\@!\|!\)"         end="$"   contains=stoneTodo,@Spell contained
syn region stoneCommentBlock             matchgroup=stoneCommentBlock         start="/\*\%(!\|\*[*/]\@!\)\@!" end="\*/" contains=stoneTodo,stoneCommentBlockNest,@Spell
syn region stoneCommentBlockDoc          matchgroup=stoneCommentBlockDoc      start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=stoneTodo,stoneCommentBlockDocNest,@Spell
syn region stoneCommentBlockDocError     matchgroup=stoneCommentBlockDocError start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=stoneTodo,stoneCommentBlockDocNestError,@Spell contained
syn region stoneCommentBlockNest         matchgroup=stoneCommentBlock         start="/\*"                     end="\*/" contains=stoneTodo,stoneCommentBlockNest,@Spell contained transparent
syn region stoneCommentBlockDocNest      matchgroup=stoneCommentBlockDoc      start="/\*"                     end="\*/" contains=stoneTodo,stoneCommentBlockDocNest,@Spell contained transparent
syn region stoneCommentBlockDocNestError matchgroup=stoneCommentBlockDocError start="/\*"                     end="\*/" contains=stoneTodo,stoneCommentBlockDocNestError,@Spell contained transparent
" FIXME: this is a really ugly and not fully correct implementation. Most
" importantly, a case like ``/* */*`` should have the final ``*`` not being in
" a comment, but in practice at present it leaves comments open two levels
" deep. But as long as you stay away from that particular case, I *believe*
" the highlighting is correct. Due to the way Vim's syntax engine works
" (greedy for start matches, unlike Rust's tokeniser which is searching for
" the earliest-starting match, start or end), I believe this cannot be solved.
" Oh you who would fix it, don't bother with things like duplicating the Block
" rules and putting ``\*\@<!`` at the start of them; it makes it worse, as
" then you must deal with cases like ``/*/**/*/``. And don't try making it
" worse with ``\%(/\@<!\*\)\@<!``, either...

syn keyword stoneTodo contained TODO FIXME XXX NB NOTE

" Folding rules {{{2
" Trivial folding rules to begin with.
" FIXME: use the AST to make really good folding
syn region stoneFoldBraces start="{" end="}" transparent fold

" Default highlighting {{{1
hi def link stoneDecNumber       stoneNumber
hi def link stoneHexNumber       stoneNumber
hi def link stoneOctNumber       stoneNumber
hi def link stoneBinNumber       stoneNumber
hi def link stoneIdentifierPrime stoneIdentifier
hi def link stoneInterface           stoneType
hi def link stoneDeriveInterface     stoneInterface

hi def link stoneMacroRepeatCount   stoneMacroRepeatDelimiters
hi def link stoneMacroRepeatDelimiters   Macro
hi def link stoneMacroVariable Define
hi def link stoneSigil         StorageClass
hi def link stoneEscape        Special
hi def link stoneEscapeUnicode stoneEscape
hi def link stoneEscapeError   Error
hi def link stoneStringContinuation Special
hi def link stoneString        String
hi def link stoneCharacterInvalid Error
hi def link stoneCharacterInvalidUnicode stoneCharacterInvalid
hi def link stoneCharacter     Character
hi def link stoneNumber        Number
hi def link stoneBoolean       Boolean
hi def link stoneEnum          stoneType
hi def link stoneEnumVariant   stoneConstant
hi def link stoneConstant      Constant
hi def link stoneThis          Constant
hi def link stoneFloat         Float
hi def link stoneArrowCharacter stoneOperator
hi def link stoneOperator      Operator
hi def link stoneKeyword       Keyword
hi def link stoneTypedef       Keyword " More precise is Typedef, but it doesn't feel right for Rust
hi def link stoneStructure     Keyword " More precise is Structure
hi def link stoneUnion         stoneStructure
hi def link stonePubScopeDelim Delimiter
hi def link stonePubScopeCrate stoneKeyword
hi def link stoneSuper         stoneKeyword
hi def link stoneReservedKeyword Error
hi def link stoneRepeat        Conditional
hi def link stoneConditional   Conditional
hi def link stoneIdentifier    Identifier
hi def link stoneCapsIdent     stoneIdentifier
hi def link stoneModPath       Include
hi def link stoneModPathSep    Delimiter
hi def link stoneFunction      Function
hi def link stoneFunctionName  Function
hi def link stoneFunCall       Function
hi def link stoneShebang       Comment
hi def link stoneCommentLine   Comment
hi def link stoneCommentLineDoc SpecialComment
hi def link stoneCommentLineDocError Error
hi def link stoneCommentBlock  stoneCommentLine
hi def link stoneCommentBlockDoc stoneCommentLineDoc
hi def link stoneCommentBlockDocError Error
hi def link stoneAssert        PreCondit
hi def link stonePanic         PreCondit
hi def link stoneMacro         Macro
hi def link stoneType          Type
hi def link stoneTodo          Todo
hi def link stoneAttribute     PreProc
hi def link stoneDerive        PreProc
hi def link stoneDefault       StorageClass
hi def link stoneStorage       StorageClass
hi def link stoneObsoleteStorage Error
hi def link stoneLifetime      Special
hi def link stoneLabel         Label
hi def link stoneInvalidBareKeyword Error
hi def link stoneExternCrate   stoneKeyword
hi def link stoneObsoleteExternMod Error
hi def link stoneBoxPlacementParens Delimiter
hi def link stoneQuestionMark  Special

" Other Suggestions:
" hi stoneAttribute ctermfg=cyan
" hi stoneDerive ctermfg=cyan
" hi stoneAssert ctermfg=yellow
" hi stonePanic ctermfg=red
" hi stoneMacro ctermfg=magenta

syn sync minlines=200
syn sync maxlines=500

let b:current_syntax = "stone"

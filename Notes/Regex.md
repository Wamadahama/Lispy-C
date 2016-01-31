Regular Expressions

We should be able to encode most of the above rules using things we know already, but Number and Program might pose some trouble. They contain a couple of constructs we've not learnt how to express yet. We don't know how to express the start or the end of input, optional characters, or range of characters.

These can be expressed, but they require something called a Regular Expression. Regular expressions are a way of writing grammars for small sections of text such as words or numbers. Grammars written using regular expressions can't consist of multiple rules, but they do give precise and concise control over what is matched and what isn't. Here are some basic rules for writing regular expressions.


- .	Any character is required.
- a	The character a is required.
- [abcdef]	Any character in the set abcdef is required.
- [a-f]	Any character in the range a to f is required.
- a?	The character a is optional.
- a*	Zero or more of the character a are required.
- a+	One or more of the character a are required.
- ^	The start of input is required.
- $	The end of input is required.

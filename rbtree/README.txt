This is a 2-pointer Red-black tree implementation.

Advantages: Only 2 pointer (16 byte on x86-64) overhead for each node,
therefore very memory efficient (compared to 3-pointer implementations).

Disadvantages:
Complexity (see implementation notes). On my machine it's slightly slower then
Niels Provos' BSD RB tree (3 pointer + 1 int metadata)

Implementation notes:

- Use of unused bits. In each node we store the color of its childs rather than
its own color. This should mean slightly less touched nodes during insertion
and deletion. In the future, we might also store the node's direction (relative
to their parent's) in the path stack in the second free bit (for 4-byte aligned
pointers). On 64-bit systems (with 8-byte aligned pointers), there is a third
free bit and we could use it to store our own color additionally (redundantly)
for even more cache efficiency.

- Path stack. This is a 2-pointer implementation. Nodes don't know their
parent. This means we have to record the path from the root to each node for
insertion and deletion. Also, this implementation is probably less suited for
concurrent writes, since the path must not be invalidated during operations.

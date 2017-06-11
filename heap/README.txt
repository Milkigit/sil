Generic (Min-) Heap. Implemented with a backing array.

Currently, NOT TESTED AT ALL! (more or less guaranteed to contain bugs)

This implementation differentiates itself from most others by the API which
makes it suitable for many more usecases:

- add and remove operations take an update callback. With that it's possible to
  track the physical position of elements on the heap.

- can remove arbitrary elements. This is nice for cancelation. That is to save
  memory by removing early, especially when it would be difficult to tell later
  whether a front-removed item is still valid.

- TODO: support updated() operation by copying the update item to an unused
  array position and then bubbling up/down. For now, just remove the item and
  then add it again.

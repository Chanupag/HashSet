#ifndef HASH_HPP_
#define HASH_HPP_

#include <list>
#include <vector>

class HashSet {
 private:
  // the number of buckets must be one of the
  // values in sizes.  After 1, they are prime numbers
  // to promote uniform hashing.  We won't test your solution
  // with more than 1'000'000 elements
  const std::vector<std::size_t> sizes {1ul, 13ul, 59ul, 127ul, 257ul, 541ul,
    1'109ul, 2'357ul, 5'087ul, 10'273ul, 20'753ul, 42'043ul,
    85'229ul, 172'933ul, 351'061ul, 712'697ul, 1'447'153ul, 2'938'679ul};

  // define the member variables you need for your solution here

  //A list is used for storing all keys.
  //which will give a contant-time iterator increment.
  std::list<int> allKeys;

  //each bucket will hold an iterator pointing to the first key in "allKeys",
  //that hashes into that bucket.
  //This can be used to find an individual key with ease.
  std::vector<std::list<int>::iterator> buckets;

  //Number of elements stored.
  std::size_t countElements;

  //Maximum loadFactor.
  //A rehash is triggered when the threshold is exceeded.
  float maxLFactor;

  //Index into sizes for the current number of buckets.
  std::size_t currentPrimeIndex;

  //Variable to declare whether the last nonempty bucket was found.
  bool found;

 public:
  // we include this line to ensure compilation with the level 2 signatures
  // you can change the way Iterator is implemented if you want
  using Iterator = std::list<int>::iterator;

  //*** Constructors, Destructor, Assignment

  // default constuctor
  HashSet();

  // copy constructor
  HashSet(const HashSet&);

  // assignment operator
  HashSet& operator=(HashSet);

  // destructor
  ~HashSet();

  //*** Core Level 1 functionality

  void insert(int key);

  bool contains(int key) const;

  void erase(int key);

  // increase number of buckets to at least newSize
  // and rehash all elements into the new buckets
  void rehash(std::size_t newSize);

  //*** Core Level 2 functionality

  Iterator find(int key);

  Iterator erase(Iterator it);

  //*** Utility functions

  // return the number of elements
  std::size_t size() const;

  // return whether or not the hash set is empty
  bool empty() const;

  // return the number of buckets, i.e. the size of the underlying array
  std::size_t bucketCount() const;

  // return the number of elements in the bucket b
  std::size_t bucketSize(std::size_t b) const;

  // return which bucket key would go in
  std::size_t bucket(int key) const;

  // return the load factor
  float loadFactor() const;

  // return the load factor threshold that provokes a rehash
  float maxLoadFactor() const;

  // set the load factor threshold
  void maxLoadFactor(float maxLoad);

  //*** Iterator Functionality

  Iterator begin();

  Iterator end();
};

#endif      // HASH_HPP_

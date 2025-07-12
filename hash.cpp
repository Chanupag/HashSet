//References:
//https://youtu.be/2_3fR-k-LzI?si=54lLnEFTdQQXHy4d
//https://en.cppreference.com/w/cpp/container/unordered_set
//https://www.youtube.com/watch?v=Fv8oj8EdssY
//https://www.youtube.com/watch?v=EBgBM7rPDic&list=PLvv0ScY6vfd8j-tlhYVPYgiIyXduu6m-L&index=39

#include <utility>
#include <list>
#include "hash.hpp"

//Iterator function: begin() and end() would return the list's begin and end.
HashSet::Iterator HashSet::begin(){
  return allKeys.begin();
}

HashSet::Iterator HashSet::end(){
  return allKeys.end();
}

//Default constructor: This will start with sizes[0] buckets.
HashSet::HashSet(): 
countElements(0), 
maxLFactor(1.0f),
currentPrimeIndex(0){
  buckets.resize(sizes[currentPrimeIndex], allKeys.end());
}

//Copy construstor: Make a copy and rebuild bucket pointers.
HashSet::HashSet(const HashSet& other):
allKeys(other.allKeys),
countElements(other.countElements),
maxLFactor(other.maxLFactor),
currentPrimeIndex(other.currentPrimeIndex) {
  buckets.resize(other.buckets.size(), allKeys.end());

  //scan through allKeys and rebuild the bucket pointers.
  for(auto it= allKeys.begin(); it!= allKeys.end(); ++it){
    std::size_t b= bucket(*it);
    if(buckets[b]== allKeys.end())
      buckets[b]= it;
  }
}
  
//Assignment operator using copy-swap.
HashSet& HashSet::operator=(HashSet other){
  std::swap(allKeys, other.allKeys);
  std::swap(buckets, other.buckets);
  std::swap(maxLFactor, other.maxLFactor);
  std::swap(currentPrimeIndex, other.currentPrimeIndex);
  std::swap(countElements, other.countElements);
  return *this;
}

//Destructor
HashSet::~HashSet(){
}

void HashSet::insert(int key){
  //Do nothing if the key is already inserted.
  if(contains(key))
    return;

  //Check if rehashing is needed.
  if(loadFactor()> maxLFactor && currentPrimeIndex+ 1<sizes.size()){
    rehash(sizes[currentPrimeIndex+ 1]);
  }
  //Determine the bucket for the key.
  std::size_t b= bucket(key);
  Iterator pos;
  //If the buckets are not empty, insert at end of its contigous block.
  if(buckets[b]!= allKeys.end()){
    pos = buckets[b];
    while(pos!= allKeys.end() && bucket(*pos)== b)
      ++pos;
  } else{
    //If the bucket is empty, look for the next non empty bucket.
    std::size_t nextBucket= b+ 1;
    while(nextBucket< buckets.size()&& buckets[nextBucket]== allKeys.end()){
      ++nextBucket;
    }
    //Insert before its first element if the bucket exist.
    if(nextBucket< buckets.size())
      pos= buckets[nextBucket];
    //Insret at the end of the list if not.
    else
      pos= allKeys.end();

  }

  //Insert the key and get the iterator to the inserted element.
  auto newIt = allKeys.insert(pos, key);

  //Update the bucket pointer if the bucket is empty.
  if (buckets[b]== allKeys.end() || std::distance(buckets[b], newIt)< 0){
    buckets[b]= newIt;
  }
  //Increase the element count.
  ++countElements;

}

//Return true if the entered key is in the set.
bool HashSet::contains(int key) const{
  //Find the bucket for the given key.
  std::size_t b= bucket(key);
  auto enteredKey= buckets[b];
  //Loop as long as the entered key is not reached the end of all keys and
  //while it hashes into the same bucket.
  while(enteredKey!= allKeys.end() && bucket(*enteredKey) == b){
    if (*enteredKey== key)
      return true;
    ++enteredKey;
  }

  return false;
}

//Return iterator to key if the key is found.
//If not found return end().
HashSet::Iterator HashSet::find(int key) {
  std::size_t b= bucket(key);
  Iterator findKey= buckets[b];
  //Loops through until the entred key has not reached the end of all keys and 
  //it still hashes into the same bucket.
  while(findKey!= allKeys.end() && bucket(*findKey)== b){
    if(*findKey== key)
      return findKey;
    ++findKey;
  }
  return allKeys.end();
}

//Erase the key with the given value.
void HashSet::erase(int key) {
  Iterator eraseKey= find(key);
  if(eraseKey!= allKeys.end())
    erase(eraseKey); 
}

//Erase given element at the iterator position 
//and return the iterator to the next element.
HashSet::Iterator HashSet::erase(HashSet::Iterator it) {
  if(it== allKeys.end())
    return it;
  int key = *it;
  std::size_t b= bucket(key);
  //If the erasing element is the first in the bucket, update the bucket pointer. 
  if(buckets[b]== it){
    Iterator nextKey= std::next(it);
    if(nextKey!= allKeys.end() && bucket(*nextKey)== b)
      buckets[b]= nextKey;
    else
      buckets[b]= allKeys.end();
  }
  Iterator returnIt = allKeys.erase(it);
  --countElements;
  return returnIt;
}

//Get a new bucket count from sizes and put all keys. 
void HashSet::rehash(std::size_t newSize) {
  //Get the new bucket count from sizes.
  std::size_t newBCount= sizes.back();
  std::size_t newPrimeIndex= currentPrimeIndex;
  for(std::size_t i= 0; i<sizes.size(); ++i){
    if(sizes[i]>= newSize){
      newBCount= sizes[i];
      newPrimeIndex= i;
      break;
    }
  }

  //Create tempory lists for buckets.
  std::vector<std::list<int>> temp(newBCount);
  for(const int key:allKeys){
    int mod= key% static_cast<int>(newBCount);
    if(mod<0) mod+= newBCount;
    std::size_t b= static_cast<std::size_t>(mod);
    temp[b].push_back(key); 
  }
  //Clear current data.
  allKeys.clear();
  buckets.clear();
  buckets.resize(newBCount, allKeys.end());
  countElements= 0;
  currentPrimeIndex= newPrimeIndex;
  //Reassamble allKeys by splicing buckets' lists.
  for(std::size_t b= 0; b<newBCount; ++b){
    if(!temp[b].empty()){
      //Store the size beforrer splicing.
      std::size_t bucketSizeBSplicing= temp[b].size();
      //Get the begining iterator. 
      Iterator bucketStart= temp[b].begin();
      allKeys.splice(allKeys.end(), temp[b]);
      //Set the bucket pointer to point the first element.
      buckets[b]= bucketStart;
      countElements+= bucketSizeBSplicing;
    }
  }
}

//Return the number of elements.
std::size_t HashSet::size() const {
  return countElements;
}

//Check and return if the hashset is empty or not.
bool HashSet::empty() const {
  return countElements== 0;
}

//Return the number of buckets.
std::size_t HashSet::bucketCount() const {
  return buckets.size();
}

//Return the size of the bucket.
std::size_t HashSet::bucketSize(std::size_t b) const {
  if(b>= buckets.size() || buckets[b]== allKeys.end())
    return 0;
  std::size_t count= 0;
  auto numKeys= buckets[b];
  while(numKeys!= allKeys.end()&& bucket(*numKeys)== b){
    ++count;
    ++numKeys;
  }
  return count;
}

//Find the bucket index for a key according to the current bucket count.  
std::size_t HashSet::bucket(int key) const {
  std::size_t bucketCount = buckets.size();
  //Adjust the modulus for negative keys.
  int mod = key % static_cast<int>(bucketCount);
  if (mod < 0)
    mod += bucketCount;
  return static_cast<std::size_t>(mod);
 
}

//Returns the current load factor.
float HashSet::loadFactor() const {
  //Compute the load factor if the bucket count is bigger than 0. Return 0.0 if not.
  //This is to avoid being divided by 0.
  return bucketCount()> 0? static_cast<float>(countElements)/ buckets.size():0.0f;
}

//Returns the max load factor thershold.
float HashSet::maxLoadFactor() const {
  return maxLFactor;
}

//Sets the max load factor.
void HashSet::maxLoadFactor(float maxLoad) {
  maxLFactor= maxLoad;

  //Trigger rehash if the max load factor is exceeded.
  if(loadFactor()> maxLFactor){
    //Loop through the list of potential sizes starting from the next index after the current prime number.
    for(std::size_t i= currentPrimeIndex+ 1; i< sizes.size(); ++i){
      //Check if the bucket size of [i] would give a load factor that is less than the maxium load factor.
      if(static_cast<float>(countElements)/ sizes[i]<= maxLFactor){
        //If so, rehash the hashset to have [i] size buckets.
        rehash(sizes[i]);
        break;
      }
    }
  }
}

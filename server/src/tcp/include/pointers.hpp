//
//  pointers.hpp
//  SysHealth
//
//  Created by Ramesh Kumar on 24/12/24.
//

#ifndef pointers_h
#define pointers_h

// --------------------------------------------------
// =======================  UNIQUE ==================
// --------------------------------------------------

template <class Type>
class Unique {
	
private:
	Type *pointer;
	
public:
	~Unique () { if (pointer) delete pointer; }
	Unique (Type * ptr = nullptr) : pointer(ptr) {}
	
	Unique (Unique & p) = delete; // delete copy constructor
	Unique & operator = (Unique & ptr) = delete; // delete copy assignment
	
	// move constructor
	Unique (Unique && ptr ) : pointer(ptr.pointer) {
		ptr.pointer = nullptr;
	}
	// move assignment
	Unique & operator = (Unique<Type> && ptr) {
		if (this != &ptr) {
			delete pointer;
			pointer = ptr.pointer;
			ptr.pointer = nullptr;
		}
		return *this;
	}
	
	// dereference
	Type & operator * () {
		return *pointer;
	}
	// pointer
	Type * operator -> () {
		return pointer;
	}
};

// --------------------------------------------------
// =======================  SHARED ==================
// --------------------------------------------------

template <class Type>
class Shared {
	
private:
	Type *pointer;
	int *count;
	
public:
	~Shared () {
		if (count) {
			if (*count > 1) {
				(*count) --;
			}
			else
			{
				delete count;
				delete pointer;
			}
		}
	}
	
	Shared (Type * ptr = nullptr) {
		if (ptr == nullptr) {
			pointer = nullptr;
			count = nullptr;
		} else {
			pointer = ptr;
			count = new int(1);
		}
	}
	
	// copy constructor
	Shared (Shared & p) {
		if (p.count) {
			(*p.count)++;
		}
		count = p.count;
		pointer = p.pointer;
	}
	
	// copy assignment
	Shared & operator = (Shared & p) {
		if ( p.pointer != this->pointer ) {
			if (count) {
				if (*count > 1) {
					*count -= 1;
				}
				else {
					delete count;
					delete pointer;
				}
			}
			*p.count += 1;
			count = p.count;
			pointer = p.pointer;
		}
		return *this;
	}
	
	// move constructor
	Shared (Shared && ptr ) {
		if (*ptr.count > 1) {
			*ptr.count -= 1;
		}
		pointer = ptr.pointer;
		count = ptr.count;
		
		ptr.pointer = nullptr;
		ptr.count = nullptr;
	}
	// move assignment
	Shared & operator = (Shared<Type> && ptr) {
		if (*ptr.count > 1) {
			*ptr.count -= 1;
		}
		pointer = ptr.pointer;
		count = ptr.count;
		ptr.pointer = nullptr;
		ptr.count = nullptr;
		return *this;
	}
	
	// dereference
	Type & operator * () {
		return *pointer;
	}
	// pointer
	Type * operator -> () {
		return pointer;
	}
	// Ref count
	int ref_count() {
		return *count;
	}
	Type * ptr() {
		return pointer;
	}
};


#endif /* pointers_h */

/**
 * @file       List.h
 * @author     Pere Tuset-Peiro (peretuset@openmote.com)
 * @version    v0.1
 * @date       May, 2016
 * @brief
 *
 * @copyright  Copyright 2016, OpenMote Technologies, S.L.
 *             This file is licensed under the GNU General Public License v2.
 */

#ifndef LIST_H_
#define LIST_H_

template<typename T>
class Item
{
public:
	Item(): next_(nullptr) {}
	T* next() {return next_;}
private:
	T* next_;
};

template<typename T>
class LinkedList
{
public:
	LinkedList(): list_(nullptr) {}
	bool addItem(T& item) {return false;}
	bool removeItem(T& item) {return false;}
	T* getFirst(void) {return list_;}
private:
	T* list_;
};

#endif /* LIST_H_ */

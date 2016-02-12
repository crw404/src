#ifndef UDPLIBRARY_UDPLINKEDLIST_H
#define UDPLIBRARY_UDPLINKEDLIST_H

// Copyright 2004 Sony Online Entertainment, all rights reserved.
// Author: Jeff Petersen

namespace UdpLibrary
{

template<typename M> class UdpLinkedListMember;
template<typename T> class UdpLinkedList;


template<typename M> class UdpLinkedListMember
{
    public:
        UdpLinkedListMember() { mPrev = NULL; mNext = NULL; }
        UdpLinkedListMember(const UdpLinkedListMember &) { mPrev = NULL; mNext = NULL; }
        ~UdpLinkedListMember() {}

#if defined(_MSC_VER) && (_MSC_VER < 1300)        // MSVC 7.0 is the first version to support friend templates
    public:
#else
    protected:
        template<typename T> friend class UdpLinkedList;
#endif
        M *mPrev;
        M *mNext;
};


template<typename T> class UdpLinkedList
{
    public:
        UdpLinkedList(UdpLinkedListMember<T> T::*node);
        virtual ~UdpLinkedList();
        
        T *First() const;
        T *Last() const;
        T *Next(const T *cur) const;
        T *Prev(const T *cur) const;
        T *Position(int index) const;
        T *Remove(T *cur);
        T *RemoveHead();
        T *RemoveTail();
        T *InsertHead(T *cur);
        T *InsertTail(T *cur);
        T *InsertAfter(T *cur, T *prev);
        int Count() const;

        void DeleteAll();       // calls delete on all contents
        void ReleaseAll();      // calls Release on all contents
        
    protected:
        T *mHead;
        T *mTail;
        UdpLinkedListMember<T> T::*mNode;
        int mCount;
};

template<typename T> UdpLinkedList<T>::UdpLinkedList(UdpLinkedListMember<T> T::*node)
{
    mHead = NULL;
    mTail = NULL;
    mNode = node;
    mCount = 0;
}

template<typename T> UdpLinkedList<T>::~UdpLinkedList()
{
}

template<typename T> T *UdpLinkedList<T>::First() const
{
    return(mHead);
}

template<typename T> T *UdpLinkedList<T>::Next(const T *cur) const
{
    return((cur->*mNode).mNext);
}

template<typename T> T *UdpLinkedList<T>::Prev(const T *cur) const
{
    return((cur->*mNode).mPrev);
}

template<typename T> T *UdpLinkedList<T>::Last() const
{
    return(mTail);
}

template<typename T> int UdpLinkedList<T>::Count() const
{
    return(mCount);
}

template<typename T> T *UdpLinkedList<T>::Position(int index) const
{
    T *cur = mHead;
    while (cur != NULL && index > 0)
    {
        cur = Next(cur);
        index--;
    }
    return(cur);
}

template<typename T> T *UdpLinkedList<T>::Remove(T *cur)
{
    UdpLinkedListMember<T> *node = &(cur->*mNode);
    if (node->mPrev == NULL)
        mHead = node->mNext;
    else
        ((node->mPrev)->*mNode).mNext = node->mNext;
        
    if (node->mNext == NULL)
        mTail = node->mPrev;
    else
        ((node->mNext)->*mNode).mPrev = node->mPrev;

    node->mNext = NULL;
    node->mPrev = NULL;
    mCount--;
    return(cur);
}

template<typename T> T *UdpLinkedList<T>::RemoveHead()
{
    if (mHead == NULL)
        return(NULL);
    return(Remove(mHead));
}

template<typename T> T *UdpLinkedList<T>::RemoveTail()
{
    if (mTail == NULL)
        return(NULL);
    return(Remove(mTail));
}

template<typename T> T *UdpLinkedList<T>::InsertHead(T *cur)
{
    assert((cur->*mNode).mPrev == NULL);
    assert((cur->*mNode).mNext == NULL);
    (cur->*mNode).mNext = mHead;

    if (mHead != NULL)
    {
        (mHead->*mNode).mPrev = cur;
        mHead = cur;
    }
    else
    {
        mTail = mHead = cur;
    }

    mCount++;
    return(cur);
}

template<typename T> T *UdpLinkedList<T>::InsertTail(T *cur)
{
    assert((cur->*mNode).mPrev == NULL);
    assert((cur->*mNode).mNext == NULL);

    (cur->*mNode).mPrev = mTail;

    if (mTail != NULL)
    {
        (mTail->*mNode).mNext = cur;
        mTail = cur;
    }
    else
    {
        mTail = mHead = cur;
    }

    mCount++;
    return(cur);
}

template<typename T> T *UdpLinkedList<T>::InsertAfter(T *cur, T *prev)
{
    assert((cur->*mNode).mPrev == NULL);
    assert((cur->*mNode).mNext == NULL);
    
    if (prev == NULL)
        return(InsertHead(cur));
        
    (cur->*mNode).mPrev = prev;
    (cur->*mNode).mNext = (prev->*mNode).mNext;
    (prev->*mNode).mNext = cur;
    
    if ((cur->*mNode).mNext != NULL)
        (((cur->*mNode).mNext)->*mNode).mPrev = cur;
    else
        mTail = cur;
        
    mCount++;
    return(cur);
}

template<typename T> void UdpLinkedList<T>::DeleteAll()
{
    T *cur = First();
    while (cur != NULL)
    {
        T *next = Next(cur);
        Remove(cur);
        delete cur;
        cur = next;
    }
}

template<typename T> void UdpLinkedList<T>::ReleaseAll()
{
    T *cur = First();
    while (cur != NULL)
    {
        T *next = Next(cur);
        Remove(cur);
        cur->Release();
        cur = next;
    }
}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<typename T> class UdpLinkedListOwn : public UdpLinkedList<T>
{
    public:
        virtual ~UdpLinkedListOwn()
        {
            this->DeleteAll();
        }
};


}   // namespace


#endif

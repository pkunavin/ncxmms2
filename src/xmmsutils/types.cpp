/**
 *  This file is a part of ncxmms2, an XMMS2 Client.
 *
 *  Copyright (C) 2011-2015 Pavel Kunavin <tusk.kun@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <xmmsclient/xmmsclient.h>
#include <string>
#include <assert.h>

#include "types.h"

using namespace ncxmms2;

/* **************************************
   ************** Dict ******************
   ************************************** */
xmms2::Variant::Type xmms2::Variant::type() const
{
    if (!m_value)
        return Type::None;
    
    switch (xmmsv_get_type(m_value)) {
        case XMMSV_TYPE_INT32:  return Type::Int;
        case XMMSV_TYPE_STRING: return Type::String;
        default:                return Type::None;
    }
}

void xmms2::Variant::getValue(int *value) const
{
    int val;
    if (m_value && xmmsv_get_int(m_value, &val))
        *value = val;
}

void xmms2::Variant::getValue(std::string *value) const
{
    const char *str;
    if (m_value && xmmsv_get_string(m_value, &str))
        *value = str;
}

void xmms2::Variant::getValue(StringRef *value) const
{
    const char *str;
    if (m_value && xmmsv_get_string(m_value, &str))
        value->assign(str);
}

xmms2::Dict::Dict(xmmsv_t *dict) :
    m_dict(dict)
{
    xmmsv_ref(m_dict);
}

xmms2::Dict::~Dict()
{
    if (m_dict)
        xmmsv_unref(m_dict);
}

void xmms2::Dict::forEach(const xmms2::Dict::ForEachFunction& func) const
{
    if (xmmsv_is_error(m_dict))
        return;
    
    xmmsv_dict_foreach(m_dict, forEachPlain, (void*)&func);
}

void xmms2::Dict::forEachPlain(const char *key, xmmsv_t *value, void *func)
{
    static_cast<ForEachFunction*>(func)->operator()(key, value);
}

xmms2::Variant xmms2::Dict::operator[](const char *key) const
{
    if (xmmsv_is_error(m_dict))
        return Variant(nullptr);
    
    xmmsv_t *dictEntry;
    return Variant(xmmsv_dict_get(m_dict, key, &dictEntry) ? dictEntry : nullptr);
}

/* **************************************
   ************** List ******************
   ************************************** */
xmms2::ListBase::ListBase(xmmsv_t *list) :
    m_list(list)
{
    xmmsv_ref(m_list);
}

xmms2::ListBase::~ListBase()
{
    if (m_list)
        xmmsv_unref(m_list);
}

int xmms2::ListBase::size() const
{
    return m_list ? xmmsv_list_get_size(m_list) : 0;
}

xmms2::ListIteratorBase::ListIteratorBase(xmmsv_t *list)
{
    if (!xmmsv_get_list_iter(list, &m_it))
        m_it = nullptr;
}

bool xmms2::ListIteratorBase::isValid() const
{
    return m_it && xmmsv_list_iter_valid(m_it);
}

void xmms2::ListIteratorBase::next()
{
    xmmsv_list_iter_next(m_it);
}

bool xmms2::ListIteratorBase::getValue(int *val) const
{
    xmmsv_t *listEntry;
    return xmmsv_list_iter_entry(m_it, &listEntry) && xmmsv_get_int(listEntry, val);
}

bool xmms2::ListIteratorBase::getValue(StringRef *val) const
{
    xmmsv_t *listEntry;
    const char *str = nullptr;
    bool ok = xmmsv_list_iter_entry(m_it, &listEntry) && xmmsv_get_string(listEntry, &str);
    val->assign(str);
    return ok;
}

bool xmms2::ListIteratorBase::getValue(xmms2::Dict *val) const
{
    xmmsv_t *listEntry;
    if (xmmsv_list_iter_entry(m_it, &listEntry)) {
        Dict dict(listEntry);
        *val = std::move(dict);
        return true;
    }
    return false;
}

/* **************************************
   ******** PlaylistChangeEvent *********
   ************************************** */
bool xmms2::PlaylistChangeEvent::init(xmmsv_t *dict)
{
    xmmsv_t *dictEntry;
    
    const char *str;
    if (!xmmsv_dict_get(dict, "name", &dictEntry) || !xmmsv_get_string(dictEntry, &str))
        return false;
    m_playlist = str;
    
    int type;
    if (!xmmsv_dict_get(dict, "type", &dictEntry) || !xmmsv_get_int(dictEntry, &type))
        return false;
    switch (type) {
        case XMMS_PLAYLIST_CHANGED_SORT:
        case XMMS_PLAYLIST_CHANGED_SHUFFLE: m_type = Type::Reorder; break;
        case XMMS_PLAYLIST_CHANGED_ADD:     m_type = Type::Add;     break;
        case XMMS_PLAYLIST_CHANGED_INSERT:  m_type = Type::Insert;  break;
        case XMMS_PLAYLIST_CHANGED_REMOVE:  m_type = Type::Remove;  break;
        case XMMS_PLAYLIST_CHANGED_CLEAR:   m_type = Type::Clear;   break;
        case XMMS_PLAYLIST_CHANGED_MOVE:    m_type = Type::Move;    break;
        default: return false;
    }
    
    if (xmmsv_dict_get(dict, "id", &dictEntry)) {
        int id;
        if (!xmmsv_get_int(dictEntry, &id))
            return false;
        m_id = id;
    }
    
    if (xmmsv_dict_get(dict, "position", &dictEntry)) {
        int position;
        if (!xmmsv_get_int(dictEntry, &position))
            return false;
        m_position = position;
    }
    
    if (xmmsv_dict_get(dict, "newposition", &dictEntry)) {
        int newPosition;
        if (!xmmsv_get_int(dictEntry, &newPosition))
            return false;
        m_newPosition = newPosition;
    }
    
    return true;
}

/* **************************************
   ******* CollectionChangeEvent ********
   ************************************** */
bool xmms2::CollectionChangeEvent::init(xmmsv_t *dict)
{
    xmmsv_t *dictEntry;
    
    const char *ns;
    if (!xmmsv_dict_get(dict, "namespace", &dictEntry) || !xmmsv_get_string(dictEntry, &ns))
        return false;
    m_kind = ns;
    
    int type;
    if (!xmmsv_dict_get(dict, "type", &dictEntry) || !xmmsv_get_int(dictEntry, &type))
        return false;
    switch (type) {
        case XMMS_COLLECTION_CHANGED_ADD:    m_type = Type::Add;    break;
        case XMMS_COLLECTION_CHANGED_UPDATE: m_type = Type::Update; break;
        case XMMS_COLLECTION_CHANGED_RENAME: m_type = Type::Rename; break;
        case XMMS_COLLECTION_CHANGED_REMOVE: m_type = Type::Remove; break;
        default: return false;
    }
    
    const char *name;
    if (!xmmsv_dict_get(dict, "name", &dictEntry) || !xmmsv_get_string(dictEntry, &name))
        return false;
    m_name = name;
    
    if (m_type == Type::Rename) {
        const char *newName;
        if (!xmmsv_dict_get(dict, "newname", &dictEntry) || !xmmsv_get_string(dictEntry, &newName))
            return false;
        m_newName = newName;
    }
    return true;
}

/* **************************************
   *********** Collection ***************
   ************************************** */
xmms2::Collection::Collection(xmms2::Collection::Type type) :
    m_coll(nullptr)
{
    xmmsv_coll_type_t collType;
    switch (type) {
        case Type::Reference:    collType = XMMS_COLLECTION_TYPE_REFERENCE;    break;
        case Type::Union:        collType = XMMS_COLLECTION_TYPE_UNION;        break;
        case Type::Intersection: collType = XMMS_COLLECTION_TYPE_INTERSECTION; break;
        case Type::Complement:   collType = XMMS_COLLECTION_TYPE_COMPLEMENT;   break;
        case Type::Has:          collType = XMMS_COLLECTION_TYPE_HAS;          break;
        case Type::Equals:       collType = XMMS_COLLECTION_TYPE_EQUALS;       break;
        case Type::Match:        collType = XMMS_COLLECTION_TYPE_MATCH;        break;
        case Type::Smaller:      collType = XMMS_COLLECTION_TYPE_SMALLER;      break;
        case Type::Greater:      collType = XMMS_COLLECTION_TYPE_GREATER;      break;
        case Type::Idlist:       collType = XMMS_COLLECTION_TYPE_IDLIST;       break;
        default:
            assert(false);
            return;
    }
    
    m_coll = xmmsv_coll_new(collType);
}

xmms2::Collection::~Collection()
{
    if (m_coll)
        xmmsv_coll_unref(m_coll);
}

void xmms2::Collection::setAttribute(const std::string& key, const std::string& value)
{
    xmmsc_coll_attribute_set(m_coll, key.c_str(), value.c_str());
}

void xmms2::Collection::addOperand(const xmms2::Collection& operand)
{
    xmmsc_coll_add_operand(m_coll, operand.m_coll);
}

xmms2::Collection xmms2::Collection::universe()
{
    return xmmsc_coll_universe();
}

xmms2::Collection xmms2::Collection::allByArtist(const std::string& artist, const xmms2::Collection& source)
{
    if (artist.empty()) {
        Collection hasArtist(Collection::Type::Has);
        hasArtist.setAttribute("field", "artist");
        hasArtist.addOperand(source);
        
        Collection hasNoArtist(Collection::Type::Complement);
        hasNoArtist.addOperand(hasArtist);
        
        Collection collection(Collection::Type::Intersection);
        collection.addOperand(hasNoArtist);
        collection.addOperand(source);
        return collection;
    }
    
    Collection collection(Collection::Type::Equals);
    collection.setAttribute("field", "artist");
    collection.setAttribute("case-sensitive", "true");
    collection.setAttribute("value", artist);
    collection.addOperand(source);
    return collection;
}

xmms2::Collection xmms2::Collection::albumByArtist(const std::string& artist, const std::string& album,
                                                   const xmms2::Collection& source)
{
    Collection artistColl = allByArtist(artist, source);
    
    if (album.empty()) {
        Collection hasAlbum(Collection::Type::Has);
        hasAlbum.setAttribute("field", "album");
        hasAlbum.addOperand(artistColl);
        
        Collection hasNoAlbum(Collection::Type::Complement);
        hasNoAlbum.addOperand(hasAlbum);
        
        Collection collection(Collection::Type::Intersection);
        collection.addOperand(hasNoAlbum);
        collection.addOperand(artistColl);
        return collection;
    }
   
    Collection collection(Collection::Type::Equals);
    collection.setAttribute("field", "album");
    collection.setAttribute("case-sensitive", "true");
    collection.setAttribute("value", album);
    collection.addOperand(artistColl);
    return collection;    
}

/* **************************************
   ************* Misc *******************
   ************************************** */
std::string xmms2::decodeUrl(const char *url)
{
    xmmsv_t *encoded, *decoded;
    const unsigned char *decodedUrl;
    unsigned int len;
    std::string decodedStr;
    
    encoded = xmmsv_new_string(url);
    decoded = xmmsv_decode_url(encoded);
    if (xmmsv_get_bin(decoded, &decodedUrl, &len)) {
        decodedStr.assign(reinterpret_cast<const char*>(decodedUrl), len);
    }    
    xmmsv_unref(encoded);
    xmmsv_unref(decoded);
    
    return decodedStr;
}

std::string xmms2::getFileNameFromUrl(const std::string& url)
{
    std::string fileName;
    const std::string::size_type slashPos = url.rfind('/');
    if (!(slashPos == std::string::npos || slashPos + 1 >= url.size()))
        fileName = url.substr(slashPos + 1);
    return fileName;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GPXParser.h"
#include "GPXextraFunc.h"
#include "LinkedListAPI.h"

/**
 * Author: Nicholas Baker
 * Date: 10/03/2021
 * Title: GPXParser.c
 * Information: This is my Functions I created for CIS*2750 Assinment 2
 */
// Assignment 2
//module 1
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile) { //Creates a valite gpx doc
    xmlDoc *doc = NULL;
    doc = xmlReadFile(fileName, NULL, 0);
    bool boolean = validXmlToXsd(doc,gpxSchemaFile); //Checks to see if the xml is valid
    if (boolean == true) {
        GPXdoc* gpxDoc = createGPXdoc(fileName); //Checks the new Gpxdoc
        if (validGPX(gpxDoc) == true) { // If all is valid
            return gpxDoc;
        } else { // GPX not valid
            deleteGPXdoc(gpxDoc);
            return NULL;
        }
    } else { //Xml is not valid
        return NULL;
    }
}

bool writeGPXdoc(GPXdoc* doc, char* fileName) { //Writes a gpx doc
    if (doc == NULL || fileName == NULL) {
        return false;
    }
    //This would test to see that the file is a .gpx
    int size = strlen(fileName);
    if (fileName[size-1] != 'x' || fileName[size-2] != 'p' || fileName[size-3] != 'g' || fileName[size-4] != '.') {
        return false;
    }
    xmlDocPtr xmlPtr = swapToDoc(doc);
    int i = xmlSaveFormatFileEnc(fileName,xmlPtr,NULL,1);
    xmlFreeDoc(xmlPtr);
    xmlCleanupParser();
    xmlMemoryDump();
    if (i == -1) {
        return false;
    }
    return true;
}

bool validateGPXDoc(GPXdoc* gpxDoc, char* gpxSchemaFile) { //Validates a gpx doc against the proper formatt and xml schema file
    if (gpxDoc == NULL || gpxSchemaFile == NULL) {
        return false;
    }
    if (validGPX(gpxDoc) == false) {
        return false;
    } else {
        xmlDocPtr xmlPtr = swapToDoc(gpxDoc);
        bool boolean = validXmlToXsd(xmlPtr,gpxSchemaFile);
        if (boolean == false) {
            return false;
        } else {
            return true;
        }
    }
}
//module 2
float round10(float len) { //Rounds a float to the nears 10
    int a = ((int)len / 10) * 10;
    int b = a + 5;
    if ((int)len >= b) {
        return (float)(a+10);
    } else {
        return (float)a;
    }
}

float getRouteLen(const Route *rt) { // gets the length of a route
    if (rt == NULL) {
        return 0;
    }

    return calculateDistance(rt->waypoints);
}

float getTrackLen(const Track *tr) { //Gets the length of a track
    if (tr == NULL) {
        return 0;
    }
    int i;
    void *elem;
    void *elem2;
    float lat, lon, total = 0;
    ListIterator iter = createIterator(tr->segments);
    if (tr->segments->length > 1) {
        for (i = 0; i < tr->segments->length; i++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (i == 0) {
                    total += calculateDistance(((TrackSegment *)elem)->waypoints);
                    elem2 = getFromBack(((TrackSegment *)elem)->waypoints);
                    lat = ((Waypoint *)elem2)->latitude;
                    lon = ((Waypoint *)elem2)->longitude;
                } else {
                    total += calculateDistance(((TrackSegment *)elem)->waypoints);
                    elem2 = getFromFront(((TrackSegment *)elem)->waypoints);
                    total += calculateDistanceLatLon(lat,lon,((Waypoint *)elem2)->latitude,((Waypoint *)elem2)->longitude);
                    elem2 = getFromBack(((TrackSegment *)elem)->waypoints);
                    lat = ((Waypoint *)elem2)->latitude;
                    lon = ((Waypoint *)elem2)->longitude;
                }
            }
        }
    } else {
        if ((elem = nextElement(&iter)) != NULL) {
            total += calculateDistance(((TrackSegment *)elem)->waypoints);
        }
    }
    return total;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta) { //Routes thats are within the length
    if (doc == NULL || len < 0 || delta < 0) {
        return 0;
    }

    void *elem;
    int counter = 0;
    float i;
    ListIterator iter = createIterator(doc->routes);
    while ((elem = nextElement(&iter)) != NULL) {
        i = getRouteLen(((Route *)elem));
        if (i < (float)(len + delta) && i > (float)(len - delta)) {
            counter++;
        }
    }
    return counter;
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta) { //Tracks that are within the length
    if (doc == NULL || len < 0 || delta < 0) {
        return 0;
    }

    void *elem;
    int counter = 0;
    float i;
    ListIterator iter = createIterator(doc->tracks);
    while ((elem = nextElement(&iter)) != NULL) {
        i = getTrackLen(((Track *)elem));
        if (i < (float)(len + delta) && i > (float)(len - delta)) {
            counter++;
        }
    }
    return counter;
}

bool isLoopRoute(const Route* route, float delta) { //Checks to see if the route is a loop
    if (route == NULL || delta < 0) {
        return false;
    }

    void *elem;
    void *elem2;
    float i;

    if (route->waypoints->length >= 4) {
        elem = getFromFront(route->waypoints);
        elem2 = getFromBack(route->waypoints);
        i = calculateDistanceLatLon(((Waypoint *)elem)->latitude,((Waypoint *)elem)->longitude, ((Waypoint *)elem2)->latitude, ((Waypoint *)elem2)->longitude);
        if (i < delta) {
            return true;
        }
        return false;
    } else {
        return false;
    }
}

bool isLoopTrack(const Track *tr, float delta) { //Checks to see if the track is a loop
    if (tr == NULL || delta < 0) {
        return false;
    }
    void *elem;
    void *elem2;
    void *elem3;
    void *elem4;
    int i,j = 0;
    float k;
    ListIterator iter = createIterator(tr->segments);
    for (i = 0; i < tr->segments->length; i++) {
        if ((elem = nextElement(&iter)) != NULL) {
            j += ((TrackSegment *)elem)->waypoints->length;
        }
    }
    if (j >= 4) {
        elem = getFromFront(tr->segments);
        elem2 = getFromBack(tr->segments);
        elem3 = getFromFront(((TrackSegment *)elem)->waypoints);
        elem4 = getFromBack(((TrackSegment *)elem2)->waypoints);
        k = calculateDistanceLatLon(((Waypoint *)elem3)->latitude,((Waypoint *)elem3)->longitude, ((Waypoint *)elem4)->latitude, ((Waypoint *)elem4)->longitude);
        if (k < delta) {
            return true;
        }
        return false;
    }
    return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) { //Checks to see if the route is between 2 points
    if (doc == NULL) {
        return NULL;
    }
    List * rte = initializeList(&routeToString, &dummyFree, &compareRoutes);
    void *elem;
    void *elem2;
    void *elem3;
    float i,j;
    ListIterator iter = createIterator(doc->routes);

    while ((elem = nextElement(&iter)) != NULL) {
        if (((Route *)elem)->waypoints->length != 0) {
            elem2 = getFromFront(((Route *)elem)->waypoints); //Start
            elem3 = getFromBack(((Route *)elem)->waypoints); //finish
            i = calculateDistanceLatLon(sourceLat,sourceLong,((Waypoint *)elem2)->latitude,((Waypoint *)elem2)->longitude); //Start
            j = calculateDistanceLatLon(destLat,destLong,((Waypoint *)elem3)->latitude,((Waypoint *)elem3)->longitude); //finish
            if (i <= delta && j <= delta) {
                insertBack(rte,elem);
            }
        }
    }
    if (rte->length > 0) {
        return rte;
    } else {
        freeList(rte);
        return NULL;
    }
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) { //Checks to see if the track is between 2 points
    if (doc == NULL) {
        return NULL;
    }
    List* trk = initializeList(&trackToString, &dummyFree, &compareTracks);
    void *elem;
    void *elem2;
    void *elem3;
    void *elem4;
    void *elem5;
    int i,j = 0;
    ListIterator iter = createIterator(doc->tracks);
    while ((elem = nextElement(&iter)) != NULL) {
        if (((Track *)elem)->segments->length != 0) {
            elem2 = getFromFront(((Track *)elem)->segments);
            elem3 = getFromBack(((Track *)elem)->segments);
            if (((TrackSegment *)elem2)->waypoints->length != 0) {
                elem4 = getFromFront(((TrackSegment *)elem2)->waypoints);
                elem5 = getFromBack(((TrackSegment *)elem3)->waypoints);
                i = calculateDistanceLatLon(sourceLat,sourceLong,((Waypoint *)elem4)->latitude,((Waypoint *)elem4)->longitude); //Start
                j = calculateDistanceLatLon(destLat,destLong,((Waypoint *)elem5)->latitude,((Waypoint *)elem5)->longitude); //finish
                if (i <= delta && j <= delta) {
                    insertBack(trk,elem);
                }
            }
        }
    }
    if (trk->length > 0) {
        return trk;
    } else {
        freeList(trk);
        return NULL;
    }
}
//module 3
char* routeToJSON(const Route *rt) { //Turns route into JSON
    if (rt == NULL) {
        size_t size = 3;
        char *str = malloc(sizeof(char) * size);
        strcpy(str,"{}");
        return str;
    }
    if (isLoopRoute(rt,10) == true) {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":true}",rt->name,rt->waypoints->length,round10(getRouteLen(rt))) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":true}",rt->name,rt->waypoints->length,round10(getRouteLen(rt)));
        return str;
    } else {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":false}",rt->name,rt->waypoints->length,round10(getRouteLen(rt))) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":false}",rt->name,rt->waypoints->length,round10(getRouteLen(rt)));
        return str;
    }
}

char* trackToJSON(const Track *tr) { //Turns Track into JSON
    if (tr == NULL) {
        size_t size = 3;
        char *str = malloc(sizeof(char) * size);
        strcpy(str,"{}");
        return str;
    }
    if (isLoopTrack(tr,10) == true) {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":true}",tr->name, getNumofTrackPoints(tr), round10(getTrackLen(tr))) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":true}",tr->name, getNumofTrackPoints(tr), round10(getTrackLen(tr)));
        return str;
    } else {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":false}",tr->name, getNumofTrackPoints(tr), round10(getTrackLen(tr))) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1lf,\"loop\":false}",tr->name, getNumofTrackPoints(tr), round10(getTrackLen(tr)));
        return str;
    }
}

char* routeListToJSON(const List *list) { //Turns a list of routes into JSON
    void *elem;
    size_t size,size2;
    char *str, *extraData, *data;
    int i;
    ListIterator iter;

    if (list == NULL) {
        str = malloc(sizeof(char) * 3);
        strcpy(str,"[]");
        return str;
    } else if (list->length == 0) {
        str = malloc(sizeof(char) * 3);
        strcpy(str,"[]");
        return str;
    }

    if (list->length == 1) {
        elem = getFromFront(((List *)list));
        data = routeToJSON(((Route *)elem));
        size = snprintf(NULL, 0, "[%s]", data) + 1;
        str = malloc(sizeof(char) * size);
        sprintf(str,"[%s]", data);
        free(data);
        return str;
    } else {
        iter = createIterator(((List *)list));
        for (i = 0; i < list->length; i++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (i == 0) {
                    data = routeToJSON(((Route *)elem));
                    size = snprintf(NULL, 0, "[%s,",data) + 1;
                    str = malloc(sizeof(char) * size);
                    sprintf(str,"[%s,", data);
                    free(data);
                } else if (i == (list->length - 1)) {
                    data = routeToJSON(((Route *)elem));
                    size2 = snprintf(NULL, 0, "%s]",data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s]", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                } else {
                    data = routeToJSON(((Route *)elem));
                    size2 = snprintf(NULL, 0, "%s,",data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s,", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                }
            }
        }
        return str;
    }
}

char* trackListToJSON(const List *list) { //Turns a list of track into JSON
    void *elem;
    size_t size,size2;
    char *str, *extraData, *data;
    int i;
    ListIterator iter;

    if (list == NULL) {
        str = malloc(sizeof(char) * 3);
        strcpy(str,"[]");
        return str;
    } else if (list->length == 0) {
        str = malloc(sizeof(char) * 3);
        strcpy(str,"[]");
        return str;
    }

    if (list->length == 1) {
        elem = getFromFront(((List *)list));
        data = trackToJSON(((Track *)elem));
        size = snprintf(NULL, 0, "[%s]", data) + 1;
        str = malloc(sizeof(char) * size);
        sprintf(str,"[%s]", data);
        free(data);
        return str;
    } else {
        iter = createIterator(((List *)list));
        for (i = 0; i < list->length; i++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (i == 0) {
                    data = trackToJSON(((Track *)elem));
                    size = snprintf(NULL, 0, "[%s,",data) + 1;
                    str = malloc(sizeof(char) * size);
                    sprintf(str,"[%s,", data);
                    free(data);
                } else if (i == (list->length - 1)) {
                    data = trackToJSON(((Track *)elem));
                    size2 = snprintf(NULL, 0, "%s]",data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s]", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                } else {
                    data = trackToJSON(((Track *)elem));
                    size2 = snprintf(NULL, 0, "%s,", data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s,", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                }
            }
        }
        return str;
    }
}

char* GPXtoJSON(const GPXdoc* gpx) { //Turns the gpx into JSON
    //{"version":ver,"creator":"crVal","numWaypoints":numW,"numRoutes":numR,"numTracks":numT}
    size_t size;
    char *str;

    if (gpx == NULL) {
        str = malloc(sizeof(char) * 3);
        strcpy(str,"{}");
        return str;
    } else {
        size = snprintf(NULL, 0, "{\"version\":%.1lf,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",gpx->version,gpx->creator,getNumWaypoints(gpx),getNumRoutes(gpx),getNumTracks(gpx)) + 1;
        str = malloc(sizeof(char) * size);
        sprintf(str,"{\"version\":%.1lf,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",gpx->version,gpx->creator,getNumWaypoints(gpx),getNumRoutes(gpx),getNumTracks(gpx));
        return str;
    }
}
//Bonus
//I only did 2 addWaypoint and addRoute
void addWaypoint(Route *rt, Waypoint *pt) {
    if (rt != NULL && pt != NULL) {
        insertBack(rt->waypoints,pt);
    }
}

void addRoute(GPXdoc* doc, Route* rt) {
    if (doc != NULL && rt != NULL) {
        insertBack(doc->routes,rt);
    }
}

GPXdoc* JSONtoGPX(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    GPXdoc *tmp = malloc(sizeof(GPXdoc));
    tmp->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    tmp->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    tmp->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    
    strcpy(tmp->namespace,"http://www.topografix.com/GPX/1/1");
    
    tmp->creator = malloc(sizeof(char) * 256);
    sscanf(gpxString,"{\"version\":%lf,\"creator\":\"%[^\"]s\"}",&tmp->version,tmp->creator);
    return tmp;
}

Waypoint* JSONtoWaypoint(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    Waypoint *tmp = malloc(sizeof(Waypoint));
    tmp->name = malloc(sizeof(char));
    tmp->name[0] = '\0';
    sscanf(gpxString,"{\"lat\":%lf,\"lon\":%lf}",&tmp->latitude,&tmp->longitude);
    tmp->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareWaypoints);
    return tmp;
}

Route* JSONtoRoute(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    Route *tmp = malloc(sizeof(Route));
    tmp->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    tmp->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareWaypoints);
    tmp->name = malloc(sizeof(char) * 256);
    sscanf(gpxString,"{\"name\":\"%[^\"]s\"}",tmp->name);
    return tmp;
}

// Assignment 1
GPXdoc* createGPXdoc(char* filename) {

    //Variables
    xmlDoc *doc = NULL;
    //Parse the file into a doc
    doc = xmlReadFile(filename, NULL, 0);
    //Check if the can be parsed
    if (doc == NULL) {
        printf("error: could not parse file %s\n", filename);
        return NULL;
    }

    xmlNode *root_element = NULL;
    xmlAttr *attr;
    GPXdoc *tmp = malloc(sizeof(GPXdoc));
    List* wpt = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    List* rte = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    List* trk = initializeList(&trackToString, &deleteTrack, &compareTracks);
    tmp->waypoints = wpt;
    tmp->routes = rte;
    tmp->tracks = trk;

    Waypoint* tmpWpt = NULL;
    Route* tmpRte = NULL;
    Track* tmpTrk = NULL;

    // Get the root element or just GPX
    root_element = xmlDocGetRootElement(doc);

    xmlNode *cur_node = NULL;
    xmlNode *cur_node2 = NULL;

    for (cur_node = root_element; cur_node != NULL; cur_node = cur_node->next) { //This is basic GPX stuff
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (strcmp("gpx",(char *)cur_node->name) == 0) {
                if (cur_node->ns->href == NULL) {
                    return NULL;
                }
                strcpy(tmp->namespace,(char *)cur_node->ns->href);
            }
        }
        tmp->version = 0.0;
        // Iterate through every attribute of the current node
        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            if (strcmp(attrName,"version") == 0) {
                tmp->version = atof(cont);
            } else if (strcmp(attrName,"creator") == 0) {
                tmp->creator = malloc(sizeof(char) * (strlen(cont) + 1));
                strcpy(tmp->creator, cont);
            }
        }
        for (cur_node2 = cur_node->children; cur_node2 != NULL; cur_node2 = cur_node2->next) {
            if (cur_node2->type == XML_ELEMENT_NODE) {
                if (strcmp("wpt",(char *)cur_node2->name) == 0) {
                    tmpWpt = createWaypoint(cur_node2);
                    if (tmpWpt == NULL) {
                        deleteGPXdoc(tmp);
                        return NULL;
                    }
                    insertBack(wpt,tmpWpt);
                } else if (strcmp("rte",(char *)cur_node2->name) == 0) {
                    tmpRte = createRoute(cur_node2);
                    if (tmpRte == NULL) {
                        deleteGPXdoc(tmp);
                        return NULL;
                    }
                    insertBack(rte,tmpRte);
                } else if (strcmp("trk",(char *)cur_node2->name) == 0) {
                    tmpTrk = createTrack(cur_node2);
                    if (tmpTrk == NULL) {
                        deleteGPXdoc(tmp);
                        return NULL;
                    }
                    insertBack(trk,tmpTrk);
                }
            }
        }
    }

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    if (tmp->creator == NULL || tmp->version == 0.0) {
        deleteGPXdoc(tmp);
        return NULL;
    }

    return tmp;
}

void deleteGPXdoc(GPXdoc* doc) { // Frees the whole doc
    if (doc != NULL) {
        free(doc->creator);
        freeList(doc->waypoints);
        freeList(doc->routes);
        freeList(doc->tracks);
        free(doc);
    }
}

char* GPXdocToString(GPXdoc* doc) { // Makes the whole doc into a to String
    size_t size = snprintf(NULL, 0, "namespace: %s\n version: %lf\n creator: %s\n", doc->namespace,doc->version, doc->creator) + 1;
    char *str = malloc(sizeof(char) * size);
    sprintf(str,"namespace: %s\n version: %lf\n creator: %s\n", doc->namespace, doc->version, doc->creator);
    char *extraData = toString(doc->waypoints);
    size = size + 50 + strlen(extraData);
    str = (char*)realloc(str, size);
    strcat(str, extraData);
    free(extraData);
    char *extraData2 = toString(doc->routes);
    size = size + 50 + strlen(extraData2);
    str = (char*)realloc(str, size);
    strcat(str, extraData2);
    free(extraData2);
    char *extraData3 = toString(doc->tracks);
    size = size + 50 + strlen(extraData3);
    str = (char*)realloc(str, size);
    strcat(str, extraData3);
    free(extraData3);
    return str;
}

//GPXData functions
void deleteGpxData(void* data) {
    GPXData *tmp = (GPXData *)data;
    free(tmp);
}

char* gpxDataToString( void* data) { //Done
    GPXData *tmp = (GPXData *)data;
    size_t size = snprintf(NULL, 0, "\t%s: %s\n", tmp->name, tmp->value) + 1;
    char *str = malloc(sizeof(char) * size);
    sprintf(str,"\t%s: %s\n", tmp->name, tmp->value);
    return str;
}

int compareGpxData(const void *first, const void *second) {
    return 0;
}

//Waypoint functions
void deleteWaypoint(void* data) {
    Waypoint *tmp = (Waypoint *)data;
    free(tmp->name);
    freeList(tmp->otherData);
    free(tmp);
}

char* waypointToString(void* data) { // Waypoint to string
    Waypoint *tmp = (Waypoint *)data;
    size_t size = snprintf(NULL, 0, "\n\tname: %s\n\tlatitude: %lf\n\tlongitude: %lf\n", tmp->name,tmp->latitude, tmp->longitude) + 1;
    char *str = malloc(sizeof(char) * size);
    sprintf(str,"\n\tname: %s\n\tlatitude: %lf\n\tlongitude: %lf\n", tmp->name,tmp->latitude, tmp->longitude);
    char *extraData = toString(tmp->otherData);
    size = size + 50 + strlen(extraData);
    str = (char*)realloc(str, size);
    strcat(str, extraData);
    free(extraData);
    return str;
}

int compareWaypoints(const void *first, const void *second) {
    return 0;
}

//Route functions
void deleteRoute(void* data) { // deletes the route
    Route *tmp = (Route *)data;
    free(tmp->name);
    freeList(tmp->waypoints);
    freeList(tmp->otherData);
    free(tmp);
}

char* routeToString(void* data) { //turns the route into a string
    Route *tmp = (Route *)data;
    size_t size = snprintf(NULL, 0, "\n\tRoute\n\tname: %s\n", tmp->name) + 1;
    char *str = malloc(sizeof(char) * size);
    sprintf(str,"\n\tRoute\n\tname: %s\n", tmp->name);
    char *extraData = toString(tmp->otherData);
    size = size + 50 + strlen(extraData);
    str = (char*)realloc(str, size);
    strcat(str, extraData);
    free(extraData);
    char *extraData2 = toString(tmp->waypoints);
    size = size + 50 + strlen(extraData2);
    str = (char*)realloc(str, size);
    strcat(str, extraData2);
    free(extraData2);
    return str;
}

int compareRoutes(const void *first, const void *second) {
    return 0;
}

//Track functions
void deleteTrack(void* data) {
    Track *tmp = (Track *)data;
    free(tmp->name);
    freeList(tmp->segments);
    freeList(tmp->otherData);
    free(tmp);
}

char* trackToString(void* data) {
    Track *tmp = (Track *)data;
    size_t size = snprintf(NULL, 0, "\n\tTrack\n\tname: %s\n", tmp->name) + 1;
    char *str = malloc(sizeof(char) * size);
    sprintf(str,"\n\tTrack\n\tname: %s\n", tmp->name);
    char *extraData = toString(tmp->otherData);
    size = size + 50 + strlen(extraData);
    str = (char*)realloc(str, size);
    strcat(str, extraData);
    free(extraData);
    char *extraData2 = toString(tmp->segments);
    size = size + 50 + strlen(extraData2);
    str = (char*)realloc(str, size);
    strcat(str, extraData2);
    free(extraData2);
    return str;
}

int compareTracks(const void *first, const void *second) {
    return 0;
}

//Track segments
void deleteTrackSegment(void* data) {
    TrackSegment *tmp = (TrackSegment *)data;
    freeList(tmp->waypoints);
    free(tmp);
}

char* trackSegmentToString(void* data) {
    TrackSegment *tmp = (TrackSegment *)data;
    return toString(tmp->waypoints);
}

int compareTrackSegments(const void *first, const void *second) {
    return 0;
}

int getNumWaypoints(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    } else {
        return getLength(doc->waypoints);
    }
}

int getNumRoutes(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    } else {
        return getLength(doc->routes);
    }
}

int getNumTracks(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    } else {
        return getLength(doc->tracks);
    }
}

int getNumSegments(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    } else {
        int c = 0;
        ListIterator iter = createIterator(doc->tracks);
	    void* elem;
	    while((elem = nextElement(&iter)) != NULL) {
            c+= ((Track *)elem)->segments->length;
	    }
        return c;
    }
}

int getNumGPXData(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    } else {
        int c = 0;
        void *elem;
        void *elem2;
        void *elem3;
        void *elem4;
        ListIterator iter = createIterator(doc->waypoints);
        ListIterator iter2;
        ListIterator iter3;
        ListIterator iter4;
	    while((elem = nextElement(&iter)) != NULL) {
            if (strcmp(((Waypoint *)elem)->name,"\0") != 0) {
                c+=1;
            }
            iter2 = createIterator(((Waypoint *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                    c+=1;
                }
            }
	    }

        iter = createIterator(doc->routes);
        while((elem = nextElement(&iter)) != NULL) {
            if (strcmp(((Route *)elem)->name,"\0") != 0) {
                c+=1;
            }
            iter2 = createIterator(((Route *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                    c+=1;
                }
            }
            iter2 = createIterator(((Route *)elem)->waypoints);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((Waypoint *)elem2)->name,"\0") != 0) {
                    c+=1;
                }
                iter3 = createIterator(((Waypoint *)elem2)->otherData);
                while((elem3 = nextElement(&iter3)) != NULL) {
                    if (strcmp(((GPXData *)elem3)->name,"\0") != 0) {
                        c+=1;
                    }
                }
            }
	    }

        iter = createIterator(doc->tracks);
        while((elem = nextElement(&iter)) != NULL) {
            if (strcmp(((Track *)elem)->name,"\0") != 0) {
                c+=1;
            }
            iter2 = createIterator(((Track *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                    c+=1;
                }
            }
            iter2 = createIterator(((Track *)elem)->segments);
            while((elem2 = nextElement(&iter2)) != NULL) {
                iter3 = createIterator(((TrackSegment *)elem2)->waypoints);
                while((elem3 = nextElement(&iter3)) != NULL) {
                    if (strcmp(((Waypoint *)elem3)->name,"\0") != 0) {
                        c+=1;
                    }
                    iter4 = createIterator(((Waypoint *)elem3)->otherData);
                    while((elem4 = nextElement(&iter4)) != NULL) {
                        if (strcmp(((GPXData *)elem4)->name,"\0") != 0) {
                            c+=1;
                        }
                    }
	            }
            }
	    }
        return c;
    }
}

Waypoint* getWaypoint(const GPXdoc* doc, char* name) { // Gets the waypoint
    if (doc == NULL) {
        return NULL;
    }
    void *elem;
    ListIterator iter = createIterator(doc->waypoints);
    while((elem = nextElement(&iter)) != NULL) {
        if (strcmp(((Waypoint *)elem)->name,name) == 0) {
            return ((Waypoint *)elem);
        }
    }
    return NULL;
}

Track* getTrack(const GPXdoc* doc, char* name) { // Gets the track
    if (doc == NULL) {
        return NULL;
    }
    void *elem;
    ListIterator iter = createIterator(doc->tracks);
    while((elem = nextElement(&iter)) != NULL) {
        if (strcmp(((Track *)elem)->name,name) == 0) {
            return ((Track *)elem);
        }
    }
    return NULL;
}

Route* getRoute(const GPXdoc* doc, char* name) { //Gets the route
    if (doc == NULL) {
        return NULL;
    }
    void *elem;
    ListIterator iter = createIterator(doc->routes);
    while((elem = nextElement(&iter)) != NULL) {
        if (strcmp(((Route *)elem)->name,name) == 0) {
            return ((Route *)elem);
        }
    }
    return NULL;
}

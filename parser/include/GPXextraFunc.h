#ifndef EXTRA_H
#define EXTRA_H
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include <libxml/parser.h>
    #include <libxml/tree.h>
    #include <libxml/encoding.h>
    #include <libxml/xmlwriter.h>
    #include <libxml/xmlschemastypes.h>
    #include "LinkedListAPI.h"
    #include "GPXParser.h"

    /**
     * Author: Nicholas Baker
     * Date: 11/02/2021
     * Title: GPXextraFunc.h
     * Information: This is my Functions I created for CIS*2750 Assinment 1
    */

    Waypoint* createWaypoint(xmlNode *cur_node);
    Route *createRoute(xmlNode *cur_node);
    Track *createTrack(xmlNode *cur_node);
    GPXData *createGPXData(xmlNode *cur_node);
    GPXData *createEmptyGPXData();
    bool validXmlToXsd(xmlDoc *doc, char* gpxSchemaFile);
    xmlDoc *swapToDoc(GPXdoc *doc);
    void addWay(xmlNodePtr root_node,GPXdoc *doc);
    void addRou(xmlNodePtr root_node, GPXdoc *doc);
    void addtrk(xmlNodePtr root_node, GPXdoc *doc);
    bool validGPX(GPXdoc *doc);
    float calculateDistance(List* list);
    float calculateDistanceLatLon(float lat1, float lon1, float lat2, float lon2 );
    void dummyFree(void *data);
    char *GPXtoJSONWrapper(char *str,char *sch);
    char *RoutetoJSONWrapper(char *str,char *sch);
    char *TracktoJSONWrapper(char *str,char *sch);
    int getNumofTrackPoints(const Track *tr);
    char* DataToJSON(const GPXData *dt);
    char* DataListToJSON(const List *list);
    char *ExtratoJSONWrapper(char *str,char *sch,char *arr);
    char *reName(char *docStr,char *sch,char *arr,char *str);
    char *JSONCreateGPXWrapper(char *docStr,char *sch,char *str);
    char *JSONCreateRoute(char *docStr,char *sch,char *str);
    char *JSONCreateWay(char *docStr,char *sch,char *Rnum, char *str);
    char *JSONBetweenRoute(char *docStr,char *sch,float sourceLat, float sourceLong, float destLat, float destLong, float delta);
    char *JSONBetweenTrack(char *docStr,char *sch,float sourceLat, float sourceLong, float destLat, float destLong, float delta);
    int RouteLength(char *docStr,char *sch, float len, float delta);
    int TrackLength(char *docStr,char *sch, float len, float delta);
    int validateWrapper(char *docStr,char *sch);
    char* waypointToJSON(const Waypoint *wp);
    char* wayListToJSON(const List *list);
    char *JSONGetWay(char *docStr,char *sch,char *Rnum);
#endif

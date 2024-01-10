#include "GPXextraFunc.h"

/**
 * Author: Nicholas Baker
 * Date: 11/02/2021
 * Title: GPXextraFunc.c
 * Information: This is my Functions I created for CIS*2750 Assinment 1
 */

Waypoint* createWaypoint(xmlNode *cur_node) {
    xmlAttr *attr;
    Waypoint *waypoint = malloc(sizeof(Waypoint));
    List *GPXData = initializeList(&gpxDataToString, &deleteGpxData, &compareWaypoints);
    int name = 0; //checks to see if the name was found
    waypoint->latitude = 0.0;
    waypoint->longitude = 0.0;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next) { //Loop to find the lat and long
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName,"lat") == 0) { // Checks for lat and sets it
            waypoint->latitude = atof(cont);
        } else if (strcmp(attrName,"lon") == 0) { // Checks for lon and sets it
            waypoint->longitude = atof(cont);
        }
    }
    xmlNode *child_node = NULL;
    for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) { //Loop to find the name and extra contents
        if (child_node->type == XML_ELEMENT_NODE) {
            if (strcmp("name",(char *)child_node->name) == 0) {
                if (child_node->children->content != NULL) {
                    waypoint->name = malloc(sizeof(char) * (strlen((char *)(child_node->children->content)) + 1));
                    strcpy(waypoint->name,(char *)child_node->children->content);
                    name++;
                }
            } else if (strcmp("name",(char *)child_node->name) != 0 && strcmp("text",(char *)child_node->name) != 0) {
                insertBack(GPXData,createGPXData(child_node));
            }
        }
    }
    if (name == 0) {
        waypoint->name = malloc(sizeof(char));
        waypoint->name[0] = '\0';
    }
    if (waypoint->longitude == 0.0 || waypoint->latitude == 0.0) {
        return NULL;
    }
    waypoint->otherData = GPXData;
    return waypoint;
}

Route *createRoute(xmlNode *cur_node) {
    xmlNode *child_node = NULL;
    Route *route = malloc(sizeof(Route));
    Waypoint *tmpWpt;
    int name = 0; //checks to see if the name was found
    int data = 0;
    List* wpt = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    List *GPXData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) { //Loop to find the name and extra contents
        if (child_node->type == XML_ELEMENT_NODE) {
            if (strcmp("name",(char *)child_node->name) == 0) {
                if (child_node->children->content != NULL) {
                    route->name = malloc(sizeof(char) * (strlen((char *)(child_node->children->content)) + 1));
                    strcpy(route->name,(char *)child_node->children->content);
                    name++;
                }
            } else if (strcmp("rtept",(char *)child_node->name) == 0) {
                tmpWpt = createWaypoint(child_node);
                if (tmpWpt == NULL) {
                    return NULL;
                }
                insertBack(wpt,tmpWpt);
            } else if (strcmp("name",(char *)child_node->name) != 0 && strcmp("text",(char *)child_node->name) != 0 && strcmp("rtept",(char *)child_node->name) != 0) {
                insertBack(GPXData,createGPXData(child_node));
                data++;
            }
        }
    }
    if (name == 0) {
        route->name = malloc(sizeof(char));
        route->name[0] = '\0';
    }
    route->waypoints = wpt;
    route->otherData = GPXData;

    return route;
}

Track *createTrack(xmlNode *cur_node) {
    xmlNode *child_node = NULL;
    xmlNode *child_node2 = NULL;
    Track *track = malloc(sizeof(Track));
    Waypoint *tmpWpt;
    TrackSegment *seg;
    List *segmentsList = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    List *wpt;
    List *GPXData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    int name = 0; //checks to see if the name was found
    track->otherData = GPXData;
    track->segments = segmentsList;

    for (child_node = cur_node->children; child_node != NULL; child_node = child_node->next) { //Loop to find the name and extra contents
        if (child_node->type == XML_ELEMENT_NODE) {
            if (strcmp("name",(char *)child_node->name) == 0) {
                if (child_node->children->content != NULL) {
                    track->name = malloc(sizeof(char) * (strlen((char *)(child_node->children->content)) + 1));
                    strcpy(track->name,(char *)child_node->children->content);
                    name++;
                }
            } else if (strcmp("trkseg",(char *)child_node->name) == 0) {
                seg = malloc(sizeof(TrackSegment));
                wpt = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
                for (child_node2 = child_node->children; child_node2 != NULL; child_node2 = child_node2->next) {
                    if (child_node2->type == XML_ELEMENT_NODE) {
                        if (strcmp("trkpt",(char *)child_node2->name) == 0) {
                            tmpWpt = createWaypoint(child_node2);
                            if (tmpWpt == NULL) {
                                return NULL;
                            }
                            insertBack(wpt,tmpWpt);
                        }
                    }
                }
                seg->waypoints = wpt;
                insertBack(segmentsList,seg);
            } else if (strcmp("name",(char *)child_node->name) != 0 && strcmp("text",(char *)child_node->name) != 0) {
                insertBack(GPXData,createGPXData(child_node));
            }
        }
    }
    if (name == 0) {
        track->name = malloc(sizeof(char));
        track->name[0] = '\0';
    }
    return track;
}

GPXData *createGPXData(xmlNode *cur_node) {
    xmlNode *child_node = NULL;
    GPXData *tmp;
    if (cur_node->children != NULL) {
        child_node = cur_node->children;
        if (child_node->content != NULL) {
            tmp = malloc(sizeof(GPXData) + sizeof(char) + (strlen((char *)child_node->content) + 1));
            strcpy(tmp->name,(char *)cur_node->name);
            strcpy(tmp->value,(char *)child_node->content);
            int i;
            for (i = 0; i <= strlen(tmp->name); i++) {
                if (tmp->name[i] == '\n' || tmp->name[i] == '\t') {
                    tmp->name[i] = ' ';
                }
            }
            for (i = 0; i <= strlen(tmp->value); i++) {
                if (tmp->value[i] == '\n' || tmp->name[i] == '\t') {
                    tmp->value[i] = ' ';
                }
            }
            //tmp->name[strlen(tmp->name)] = '\0';
            //tmp->value[strlen(tmp->value)] = '\0';
        } else {
            tmp = malloc(sizeof(GPXData) + sizeof(char));
            strcpy(tmp->name,(char *)cur_node->name);
            tmp->value[0] = '\0';
        }
    } else {
        tmp = malloc(sizeof(GPXData) + sizeof(char));
        strcpy(tmp->name,(char *)cur_node->name);
        tmp->value[0] = '\0';
    }
    return tmp;
}

GPXData *createEmptyGPXData() {
    GPXData *tmp = malloc(sizeof(GPXData) + sizeof(char));
    tmp->name[0] = '\0';
    tmp->value[0] = '\0';
    return tmp;
}

bool validXmlToXsd(xmlDoc *doc, char* gpxSchemaFile) { //Checks if a xml doc is valid
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    if (doc == NULL) {
        return false;
    }

    xmlSchemaValidCtxtPtr valid;
    int ret;

    valid = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(valid, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    ret = xmlSchemaValidateDoc(valid, doc);
    xmlSchemaFreeValidCtxt(valid);
    xmlFreeDoc(doc);

    // free the resource
    if (schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    if (ret == 0) {
        return true;
    } else if (ret > 0) {
        return false;
    }
    return false;
}

xmlDoc *swapToDoc(GPXdoc *doc) { //Swaps a gpx to a xml doc
    xmlDocPtr swap = NULL;
    xmlNsPtr ns = NULL;
    xmlNodePtr root_node = NULL;
    char buff[256];

    swap = xmlNewDoc(BAD_CAST "1.0");

    //Set GPX info
    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    sprintf(buff,"%.1lf",doc->version);
    xmlNewProp(root_node,BAD_CAST "version",BAD_CAST buff);
    xmlNewProp(root_node,BAD_CAST "creator",BAD_CAST doc->creator);
    xmlDocSetRootElement(swap, root_node);
    ns = xmlNewNs(root_node,BAD_CAST doc->namespace, NULL);
    xmlSetNs(root_node, ns);

    addWay(root_node,doc);
    addRou(root_node,doc);
    addtrk(root_node,doc);

    return swap;
}

void addWay(xmlNodePtr root_node,GPXdoc *doc) { //Adds waypoints to a xml
    void *elem;
    void *elem2;
    char buff[256];
    xmlNodePtr node = NULL;
    ListIterator iter = createIterator(doc->waypoints);
    ListIterator iter2;
    while((elem = nextElement(&iter)) != NULL) {
        node = xmlNewChild(root_node, NULL, BAD_CAST "wpt", NULL);
        sprintf(buff,"%lf",((Waypoint *)elem)->latitude);
        xmlNewProp(node,BAD_CAST "lat",BAD_CAST buff);
        sprintf(buff,"%lf",((Waypoint *)elem)->longitude);
        xmlNewProp(node,BAD_CAST "lon",BAD_CAST buff);
        if (strcmp(((Waypoint *)elem)->name,"\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Waypoint *)elem)->name);
        }
        iter2 = createIterator(((Waypoint *)elem)->otherData);
        while((elem2 = nextElement(&iter2)) != NULL) {
            if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                xmlNewChild(node, NULL, BAD_CAST ((GPXData *)elem2)->name, BAD_CAST ((GPXData *)elem2)->value);
            }
        }
    }
}

void addRou(xmlNodePtr root_node, GPXdoc *doc) { //Adds routes to a xml
    void *elem;
    void *elem2;
    char buff[256];
    xmlNodePtr node = NULL;
    xmlNodePtr node1 = NULL;
    ListIterator iter = createIterator(doc->routes);
    ListIterator iter2;
    while((elem = nextElement(&iter)) != NULL) {
        node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);
        if (strcmp(((Route *)elem)->name,"\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Route *)elem)->name);
        }
        iter2 = createIterator(((Route *)elem)->otherData);
        while((elem2 = nextElement(&iter2)) != NULL) {
            if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                xmlNewChild(node, NULL, BAD_CAST ((GPXData *)elem2)->name, BAD_CAST ((GPXData *)elem2)->value);
            }
        }
        iter2 = createIterator(((Route *)elem)->waypoints);
        while((elem2 = nextElement(&iter2)) != NULL) {
            node1 = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);
            sprintf(buff,"%lf",((Waypoint *)elem2)->latitude);
            xmlNewProp(node1,BAD_CAST "lat",BAD_CAST buff);
            sprintf(buff,"%lf",((Waypoint *)elem2)->longitude);
            xmlNewProp(node1,BAD_CAST "lon",BAD_CAST buff);
            if (strcmp(((Waypoint *)elem2)->name,"\0") != 0) {
                xmlNewChild(node1, NULL, BAD_CAST "name", BAD_CAST ((Waypoint *)elem)->name);
            }
        }
    }
}

void addtrk(xmlNodePtr root_node, GPXdoc *doc) { //Adds tracks to a xml
    void *elem;
    void *elem2;
    void *elem3;
    void *elem4;
    char buff[256];
    xmlNodePtr node = NULL;
    xmlNodePtr node1 = NULL;
    xmlNodePtr node2 = NULL;
    ListIterator iter = createIterator(doc->waypoints);
    ListIterator iter2;
    ListIterator iter3;
    ListIterator iter4;
    iter = createIterator(doc->tracks);
    while((elem = nextElement(&iter)) != NULL) {
        node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);
        if (strcmp(((Track *)elem)->name,"\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Track *)elem)->name);
        }
        iter2 = createIterator(((Track *)elem)->otherData);
        while((elem2 = nextElement(&iter2)) != NULL) {
            if (strcmp(((GPXData *)elem2)->name,"\0") != 0) {
                xmlNewChild(node, NULL, BAD_CAST ((GPXData *)elem2)->name, BAD_CAST ((GPXData *)elem2)->value);
            }
        }
        iter2 = createIterator(((Track *)elem)->segments);
        while((elem2 = nextElement(&iter2)) != NULL) {
            node1 = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
            iter3 = createIterator(((TrackSegment *)elem2)->waypoints);
            while((elem3 = nextElement(&iter3)) != NULL) {
                node2 = xmlNewChild(node1, NULL, BAD_CAST "trkpt", NULL);
                sprintf(buff,"%lf",((Waypoint *)elem3)->latitude);
                xmlNewProp(node2,BAD_CAST "lat",BAD_CAST buff);
                sprintf(buff,"%lf",((Waypoint *)elem3)->longitude);
                xmlNewProp(node2,BAD_CAST "lon",BAD_CAST buff);
                if (strcmp(((Waypoint *)elem3)->name,"\0") != 0) {
                    xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST ((Waypoint *)elem3)->name);
                }
                iter4 = createIterator(((Waypoint *)elem3)->otherData);
                while((elem4 = nextElement(&iter4)) != NULL) {
                    if (strcmp(((GPXData *)elem4)->name,"\0") != 0) {
                        xmlNewChild(node2, NULL, BAD_CAST ((GPXData *)elem4)->name, BAD_CAST ((GPXData *)elem4)->value);
                    }
                }
            }
        }
    }
}

bool validGPX(GPXdoc *doc) { //This will check if a gpx is valid
    if (doc == NULL) {
        return false;
    } else {
        void *elem;
        void *elem2;
        void *elem3;
        void *elem4;

        if (doc->waypoints == NULL) {
            return false;
        }

        ListIterator iter = createIterator(doc->waypoints);
        ListIterator iter2;
        ListIterator iter3;
        ListIterator iter4;

	    while((elem = nextElement(&iter)) != NULL) {
            if (((Waypoint *)elem)->latitude <= -90 || ((Waypoint *)elem)->latitude >= 90) {
                return false;
            } else if (((Waypoint *)elem)->longitude <= -180 || ((Waypoint *)elem)->longitude >= 180) {
                return false;
            }
            if (((Waypoint *)elem)->otherData == NULL) {
                return false;
            }
            iter2 = createIterator(((Waypoint *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") == 0) {
                    return false;
                }
            }
	    }
        if (doc->routes == NULL) {
            return false;
        }
        iter = createIterator(doc->routes);
        while((elem = nextElement(&iter)) != NULL) {
            if (((Route *)elem)->otherData == NULL) {
                return false;
            }
            iter2 = createIterator(((Route *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") == 0) {
                    return false;
                }
            }
            if (((Route *)elem)->waypoints == NULL) {
                return false;
            }
            iter2 = createIterator(((Route *)elem)->waypoints);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (((Waypoint *)elem2)->latitude <= -90 || ((Waypoint *)elem2)->latitude >= 90) {
                    return false;
                } else if (((Waypoint *)elem2)->longitude <= -180 || ((Waypoint *)elem2)->longitude >= 180) {
                    return false;
                }
                if (((Waypoint *)elem2)->otherData == NULL) {
                    return false;
                }
                iter3 = createIterator(((Waypoint *)elem2)->otherData);
                while((elem3 = nextElement(&iter3)) != NULL) {
                    if (strcmp(((GPXData *)elem3)->name,"\0") == 0) {
                        return false;
                    }
                }
            }
	    }
        if (doc->tracks == NULL) {
            return false;
        }
        iter = createIterator(doc->tracks);
        while((elem = nextElement(&iter)) != NULL) {
            if (((Track *)elem)->otherData == NULL) {
                return false;
            }
            iter2 = createIterator(((Track *)elem)->otherData);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (strcmp(((GPXData *)elem2)->name,"\0") == 0) {
                    return false;
                }
            }
            if (((Track *)elem)->segments == NULL) {
                return false;
            }
            iter2 = createIterator(((Track *)elem)->segments);
            while((elem2 = nextElement(&iter2)) != NULL) {
                if (((TrackSegment *)elem2)->waypoints == NULL) {
                    return false;
                }
                iter3 = createIterator(((TrackSegment *)elem2)->waypoints);
                while((elem3 = nextElement(&iter3)) != NULL) {
                    if (((Waypoint *)elem3)->latitude <= -90 || ((Waypoint *)elem3)->latitude >= 90) {
                        return false;
                    } else if (((Waypoint *)elem3)->longitude <= -180 || ((Waypoint *)elem3)->longitude >= 180) {
                        return false;
                    }
                    if (((Waypoint *)elem3)->otherData == NULL) {
                        return false;
                    }
                    iter4 = createIterator(((Waypoint *)elem3)->otherData);
                    while((elem4 = nextElement(&iter4)) != NULL) {
                        if (strcmp(((GPXData *)elem4)->name,"\0") == 0) {
                            return false;
                        }
                    }
	            }
            }
	    }
    }
    return true;
}

float calculateDistance(List* list) { //This will calculate the distance of a list
    /*φ is latitude, λ is longitude, R is earth’s radius (mean radius = 6,371km);
    note that angles need to be in radians to pass to trig functions!*/

    /*const R = 6371e3; metres
    const φ1 = lat1 * Math.PI/180;  φ, λ in radians
    const φ2 = lat2 * Math.PI/180;
    const Δφ = (lat2-lat1) * Math.PI/180;
    const Δλ = (lon2-lon1) * Math.PI/180;

    const a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
              Math.cos(φ1) * Math.cos(φ2) *
              Math.sin(Δλ/2) * Math.sin(Δλ/2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

    const d = R * c;  in metres */

    if (list == NULL) {
        return 0;
    }
    void *elem;
    void *elem2;
    float lat1, lat2, disp1, disp2, total = 0;
    double a, c, d, r = 6371000;
    ListIterator iter = createIterator(list);
    ListIterator iter2 = createIterator(list);
    ListIterator iter3 = createIterator(list);

    if (nextElement(&iter3) != NULL) {
        elem2 = nextElement(&iter2);
        while((elem = nextElement(&iter)) != NULL) {
            if ((elem2 = nextElement(&iter2)) != NULL) {
                lat1 = ((Waypoint *)elem)->latitude * M_PI/180;
                lat2 = ((Waypoint *)elem2)->latitude * M_PI/180;
                disp1 = (((Waypoint *)elem2)->latitude - ((Waypoint *)elem)->latitude) * M_PI/180;
                disp2 = (((Waypoint *)elem2)->longitude - ((Waypoint *)elem)->longitude) * M_PI/180;
                a = (sin((double)(disp1/2)) * sin((double)(disp1/2))) + (cos((double)(lat1)) * cos((double)(lat2)) * sin((double)(disp2/2)) * sin((double)(disp2/2)));
                c = 2 * atan2(sqrt(a), sqrt(1-a));
                d = r * c;
                total += d;
            }
        }
    }
    return total;
}

float calculateDistanceLatLon(float lat1, float lon1, float lat2, float lon2 ) { //This will calculate from 1 distance to another
    /*φ is latitude, λ is longitude, R is earth’s radius (mean radius = 6,371km);
    note that angles need to be in radians to pass to trig functions!*/

    /*const R = 6371e3; metres
    const φ1 = lat1 * Math.PI/180;  φ, λ in radians
    const φ2 = lat2 * Math.PI/180;
    const Δφ = (lat2-lat1) * Math.PI/180;
    const Δλ = (lon2-lon1) * Math.PI/180;

    const a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
              Math.cos(φ1) * Math.cos(φ2) *
              Math.sin(Δλ/2) * Math.sin(Δλ/2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

    const d = R * c;  in metres */
    float x, y, disp1, disp2;
    double a, c, r = 6371000;
    x = lat1 * M_PI/180;
    y = lat2 * M_PI/180;
    disp1 = (lat2 - lat1) * M_PI/180;
    disp2 = (lon2 - lon1) * M_PI/180;
    a = (sin((double)(disp1/2)) * sin((double)(disp1/2))) + (cos((double)(x)) * cos((double)(y)) * sin((double)(disp2/2)) * sin((double)(disp2/2)));
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    return (r * c);
}

//dummy
void dummyFree(void *data) {}

int getNumofTrackPoints(const Track *tr) {
    int num = 0;
    void *elem;
    ListIterator iter = createIterator(tr->segments);
    while((elem = nextElement(&iter)) != NULL) {
        num += ((TrackSegment *)elem)->waypoints->length;
    }
    return num;
}

//Wrapper Functions
char *GPXtoJSONWrapper(char *str,char *sch) {
    GPXdoc* doc = createValidGPXdoc(str,sch);
    char *JSON = GPXtoJSON(doc);
    deleteGPXdoc(doc);
    return JSON;
}

char *RoutetoJSONWrapper(char *str,char *sch) {
    GPXdoc* doc = createValidGPXdoc(str,sch);
    char *JSON = routeListToJSON(doc->routes);
    deleteGPXdoc(doc);
    return JSON;
}

char *TracktoJSONWrapper(char *str,char *sch) {
    GPXdoc* doc = createValidGPXdoc(str,sch);
    char *JSON = trackListToJSON(doc->tracks);
    deleteGPXdoc(doc);
    return JSON;
}

char *ExtratoJSONWrapper(char *str,char *sch,char *arr) {
    GPXdoc* doc = createValidGPXdoc(str,sch);
    void *elem;
    char type[256];
    int i,j;
    char *data;
    ListIterator iter;
    sscanf(arr,"%s %d",type, &i);
    if (strcmp(type,"Route") == 0) {
        iter = createIterator(doc->routes);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    data = DataListToJSON(((Route *)elem)->otherData);
                }
            }
        }
    } else if (strcmp(type,"Track") == 0) {
        iter = createIterator(doc->tracks);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    data = DataListToJSON(((Track *)elem)->otherData);
                }
            }
        }
    }
    deleteGPXdoc(doc);
    return data;
    //List* otherData
}

char *reName(char *docStr,char *sch,char *arr,char *str) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    void *elem;
    char type[256];
    int i,j;
    ListIterator iter;
    sscanf(arr,"%s %d",type, &i);
    if (strcmp(type,"Route") == 0) {
        iter = createIterator(doc->routes);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    strcpy(((Route*)elem)->name,str);
                }
            }
        }
    } else if (strcmp(type,"Track") == 0) {
        iter = createIterator(doc->tracks);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    strcpy(((Track *)elem)->name,str);
                }
            }
        }
    }
    if (validateGPXDoc(doc,sch) == true) {
        writeGPXdoc(doc,docStr);
        deleteGPXdoc(doc);
        return "true";
    } else {
        deleteGPXdoc(doc);
        return "false";
    }
    
    //List* otherData
}

char *JSONCreateGPXWrapper(char *docStr,char *sch,char *str) {
    GPXdoc* doc = JSONtoGPX(str);
    if (validateGPXDoc(doc,sch) == true && writeGPXdoc(doc,docStr) == true) {
        deleteGPXdoc(doc);
        return "true";
    } else {
        deleteGPXdoc(doc);
        return "false";
    }
}

char *JSONCreateRoute(char *docStr,char *sch,char *str) {
    GPXdoc *doc = createValidGPXdoc(docStr,sch);
    Route *rte = JSONtoRoute(str);
    insertBack(doc->routes,rte);
    if (validateGPXDoc(doc,sch) == true) {
        writeGPXdoc(doc,docStr);
        deleteGPXdoc(doc);
        return "true";
    } else {
        deleteGPXdoc(doc);
        return "false";
    }
}

char *JSONCreateWay(char *docStr,char *sch,char *Rnum, char *str) {
    GPXdoc *doc = createValidGPXdoc(docStr,sch);
    Waypoint *rte = JSONtoWaypoint(str);
    void *elem;
    char type[256];
    int i,j;
    ListIterator iter;
    sscanf(Rnum,"%s %d",type, &i);
    if (strcmp(type,"Route") == 0) {
        iter = createIterator(doc->routes);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    insertBack(((Route *)elem)->waypoints,rte);
                }
            }
        }
    }
    if (validateGPXDoc(doc,sch) == true && writeGPXdoc(doc,docStr) == true) {
        deleteGPXdoc(doc);
        return "true";
    } else {
        deleteGPXdoc(doc);
        return "false";
    }
}

char *JSONBetweenRoute(char *docStr,char *sch,float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    List *rte = getRoutesBetween(doc,sourceLat,sourceLong,destLat,destLong,delta);
    char *str = routeListToJSON(rte);
    deleteGPXdoc(doc);
    return str;
}

char *JSONBetweenTrack(char *docStr,char *sch,float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    List *rte = getTracksBetween(doc,sourceLat,sourceLong,destLat,destLong,delta);
    char *str = trackListToJSON(rte);
    deleteGPXdoc(doc);
    return str;
}

int validateWrapper(char *docStr,char *sch) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    if (doc == NULL) {
        return 0; //False not valid
    } else {
        deleteGPXdoc(doc);
        return 1; //True the file is valid
    }
}

int RouteLength(char *docStr,char *sch, float len, float delta) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    int i = numRoutesWithLength(doc,len,delta);
    deleteGPXdoc(doc);
    return i;
}

int TrackLength(char *docStr,char *sch, float len, float delta) {
    GPXdoc* doc = createValidGPXdoc(docStr,sch);
    int i = numTracksWithLength(doc,len,delta);
    deleteGPXdoc(doc);
    return i;
}
//Data to JSON
char* waypointToJSON(const Waypoint *wp) { //Turns route into JSON
    if (wp == NULL) {
        size_t size = 3;
        char *str = malloc(sizeof(char) * size);
        strcpy(str,"{}");
        return str;
    } else {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"longitude\":%lf,\"latitude\":%lf}",wp->name,wp->longitude,wp->latitude) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"longitude\":%lf,\"latitude\":%lf}",wp->name,wp->longitude,wp->latitude);
        return str;
    }
}

char* wayListToJSON(const List *list) { //Turns a list of routes into JSON
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
        data = waypointToJSON(((Waypoint *)elem));
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
                    data =  waypointToJSON(((Waypoint *)elem));
                    size = snprintf(NULL, 0, "[%s,",data) + 1;
                    str = malloc(sizeof(char) * size);
                    sprintf(str,"[%s,", data);
                    free(data);
                } else if (i == (list->length - 1)) {
                    data =  waypointToJSON(((Waypoint *)elem));
                    size2 = snprintf(NULL, 0, "%s]",data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s]", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                } else {
                    data =  waypointToJSON(((Waypoint *)elem));
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

char *JSONGetWay(char *docStr,char *sch,char *Rnum) {
    GPXdoc *doc = createValidGPXdoc(docStr,sch);
    void *elem;
    char type[256];
    int i,j;
    char *str = NULL;
    ListIterator iter;
    sscanf(Rnum,"%s %d",type, &i);
    if (strcmp(type,"Route") == 0) {
        iter = createIterator(doc->routes);
        for (j = 0; j < i; j++) {
            if ((elem = nextElement(&iter)) != NULL) {
                if (j == (i-1)){
                    str = wayListToJSON(((Route *)elem)->waypoints);
                }
            }
        }
    }
    if (str == NULL) {
        deleteGPXdoc(doc);
        return "[]";
    } else {
        deleteGPXdoc(doc);
        return str;
    }
}

char* DataToJSON(const GPXData *dt) { //Turns route into JSON
    if (dt == NULL) {
        size_t size = 3;
        char *str = malloc(sizeof(char) * size);
        strcpy(str,"{}");
        return str;
    } else {
        size_t size = snprintf(NULL, 0, "{\"name\":\"%s\",\"value\":\"%s\"}",dt->name,dt->value) + 1;
        char *str = malloc(sizeof(char) * size);
        sprintf(str,"{\"name\":\"%s\",\"value\":\"%s\"}",dt->name,dt->value);
        return str;
    }
}

char* DataListToJSON(const List *list) { //Turns a list of routes into JSON
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
        data = DataToJSON(((GPXData *)elem));
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
                    data =  DataToJSON(((GPXData *)elem));
                    size = snprintf(NULL, 0, "[%s,",data) + 1;
                    str = malloc(sizeof(char) * size);
                    sprintf(str,"[%s,", data);
                    free(data);
                } else if (i == (list->length - 1)) {
                    data =  DataToJSON(((GPXData *)elem));
                    size2 = snprintf(NULL, 0, "%s]",data) + 1;
                    extraData = malloc(sizeof(char) * size2);
                    sprintf(extraData,"%s]", data);
                    size = size + 50 + size2;
                    str = (char*)realloc(str, size);
                    strcat(str,extraData);
                    free(extraData);
                    free(data);
                } else {
                    data =  DataToJSON(((GPXData *)elem));
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
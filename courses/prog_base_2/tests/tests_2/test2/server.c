#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> //  ptrdiff_t
#include <ctype.h>
#include "student.h"
#include "server.h"
#include "socket.h"
#include "Freelanser.h"
#include "teacher.h"
#include "Databse.h"
#include "list.h"
teacher teachers[1];
void server_database(socket_t * client) {
	char strbuf[10240] = "";
	char buf[1000];/*
	FILE * file = fopen("Untitled1.xml", "r");
	while(fgets(buf, 1000, file) != NULL)
	{
		strcat(strbuf, buf);
		puts(buf);
	}
	*/
	db_t * db = db_new("admin.db");
	list_t list = database_to_list(db);
	puts(((struct admin_s *)list_get(list, 0))->name);
	char * text = list_to_xml1(list);
	sprintf(strbuf,
		"HTTP/1.1 200 OK\n"
		"Content-Type: text/xml\n"
		"Content-Length: %u\n"
		"Connection: keep-alive\n"
		"\n%s", strlen(text), text);
	//free(allStudentsJson);
	free (text);
	//fclose(file);
    socket_write_string(client, strbuf);
    socket_close(client);
}
void fill_teach(){
int p = teacher_fillArray(1,teachers);
}

void server_answer(http_request_t req, socket_t * clientSocket,teacher_t ** freelanser,char * myJ)
{
    puts(req.method);
    puts(req.uri);
    if (!strcmp(req.uri, "/teacher-count"))
    {
        server_count(clientSocket,freelanser);
    }
    else if(!strcmp(req.uri, "/info"))
    {

	 char homeBuf[1024];

    cJSON * jInfo = cJSON_CreateObject();
    cJSON_AddItemToObject(jInfo, "student", cJSON_CreateString("Behrang Behvandi"));
    cJSON_AddItemToObject(jInfo, "group", cJSON_CreateString("KP-51"));
    cJSON_AddItemToObject(jInfo, "variant", cJSON_CreateNumber(9));
    char * pageText = cJSON_Print(jInfo);

    sprintf(homeBuf,
            "HTTP/1.1 404 \n"
            "Content-Type: text/html/application/json\n"
            "Content-Length: %zu\n"
            "\n%s", strlen(pageText), pageText);

    socket_write_string(clientSocket,homeBuf);
    }

    else if(!strcmp(req.uri, "/external"))
    {
    char buf[10000];
    sprintf(buf,"\nHTTP1.1 200 OK\n"
    "Content-Type: application/json\n"
    "Content-Length: %i\r\n\r\n"
    "%s\n",strlen(myJ),myJ);
    socket_write_string(clientSocket,buf);
    }
     else if(!strcmp(req.uri, "/database"))
    {
        server_database(clientSocket);
    }
    else
    if (!strcmp(req.uri, "/teacher/api"))
    {
        if (!strcmp(req.method, "GET"))
            server_GET_all(req,clientSocket,freelanser);
        else if(!strcmp(req.method, "DELETE"))
            server_DELETE_all(req,clientSocket,freelanser);
        else
            socket_write_string(clientSocket, "Method not allowed!\n");

    }
    else if (strncmp(req.uri, "/teacher/api/",13)==0)
    {
        if(!strcmp(req.method,"GET"))
            server_GET_id(req,clientSocket,freelanser);
        else if(!strcmp(req.method,"DELETE"))
            server_DELETE_id(req,clientSocket,freelanser);
        else if(!strcmp(req.method,"POST"))
        {
            puts("1");
            server_POST(req,clientSocket,freelanser);
        }
    }
    else if(!strncmp(req.uri,"/teacher/delete/",16))
    {
        server_DELETE_id(req,clientSocket,freelanser);
    }
    else  if(!strcmp(req.uri,"/teacher"))
    {
        server_start_page(clientSocket);
    }
    else if(!strcmp(req.uri,"/teacher/"))
    {
        server_GET_html(clientSocket,freelanser);
    }
    else if(!strcmp(req.uri,"/teacher/paste"))
    {
        server_Paste_html(req,clientSocket,freelanser);
    }
    else if(!strncmp(req.uri,"/teacher/",9))
    {
        server_GET_id_html(req,clientSocket,freelanser);
    }
    else
        socket_write_string(clientSocket,"\nHTTP1.1 404 NOT FOUND\n"
            "Content-Type: application/json\n"
            "Content-Length: %i\r\n\r\n""Freelancer not found\n");
}


void server_sent(socket_t* clientSocket, char* text)
{
    char buf[10000];
    sprintf(buf,"\nHTTP1.1 200 OK\n"
            "Content-Type: application/json\n"
            "Content-Length: %i\r\n\r\n"
            "%s\n",strlen(text),text);
    socket_write_string(clientSocket,buf);
}

char * lanser_toJSON(teacher_t * self)
{
    cJSON * SM = cJSON_CreateObject();
    cJSON_AddItemToObject(SM, "name", cJSON_CreateString(teacher_name_get(self)));
    cJSON_AddItemToObject(SM, "surname", cJSON_CreateString(teacher_surname_get(self)));
    cJSON_AddItemToObject(SM, "age", cJSON_CreateNumber(teacher_age_get(self)));
    //cJSON_AddItemToObject(SM, "date", cJSON_CreateString(teacher_date_get(self)));
    cJSON_AddItemToObject(SM, "salary", cJSON_CreateNumber(teacher_salary_get(self)));
    char * jsonSM = cJSON_Print(SM);
    return jsonSM;
}

void server_count( socket_t * clientSocket,teacher_t** freelancer)
{
    int count=0;
    for(int i=0;i<10;i++)
        if(strcmp(teacher_name_get(freelancer[i]),"")!=0)
            count++;
    cJSON * SM = cJSON_CreateObject();
    cJSON_AddItemToObject(SM, "count", cJSON_CreateNumber(count));
    char* text =cJSON_Print(SM);
    server_sent(clientSocket,text);
}

void server_GET_all(http_request_t req, socket_t * clientSocket,teacher_t** freelancer)
{
    char text[10000]="";
    char * buf=NULL;
    int count=0,check=0;

    for(int i = 0; i < 10; i++)
            if(strcmp(teacher_name_get(freelancer[i]),"")!=0)
                count++;


        strcat(text, "[");
        for(int i = 0; i < 10; i++)
        {
            if(strcmp(teacher_name_get(freelancer[i]),"")==0)
            {
                i++;
            }
            else
            {
                buf = lanser_toJSON(freelancer[i]);

            strcat(text, buf);
            if(check != count-1)
            {
                strcat(text, ",");
                check++;
            }
            }
        }
        strcat(text, "]");

    server_sent(clientSocket, text);
}

void server_GET_id(http_request_t req,socket_t * clientSocket,teacher_t** freelancer)
{
    char text[1000]="";
    char *buf=NULL;
    int id;

    if(strpbrk(req.uri,"-0123456789"))
    {
        id=atoi(strpbrk(req.uri,"-0123456789"));
        if(id>9 || id<0)
        {
            socket_write_string(clientSocket,"HTTP1.1 404 NOT FOUND\n"
            "Content-Type: json\n"
            "Content-Length: %i\r\n\r\n"
            "Id not found");
            return;
        }
    }
    else
    {
        socket_write_string(clientSocket,"Wrong id");
        return;
    }


            buf = lanser_toJSON(freelancer[id]);

            strcat(text, buf);

    server_sent(clientSocket, text);

}

void server_DELETE_all(http_request_t req,socket_t * clientSocket,teacher_t** freelanser)
{
    for(int i=0; i<10; i++)
    {
        //teacher_set(freelanser[i],NULL,NULL,0,NULL,0);
        teacher_set(freelanser[i],NULL,NULL,0,0,0);
    }
    server_sent(clientSocket, "Delete success");
}

void server_DELETE_id(http_request_t req,socket_t * clientSocket,teacher_t** freelanser)
{
    char buf[1000]="";
    int id;

    if(strpbrk(req.uri,"-0123456789"))
    {
        id=atoi(strpbrk(req.uri,"-0123456789"));
        if(id>9 || id<0)
        {
            socket_write_string(clientSocket,"HTTP1.1 404 NOT FOUND\n"
            "Content-Type: json\n"
            "Content-Length: %i\r\n\r\n"
            "%s\n""<body>Id not found<br>"
                                  "<a href=\"/teacher/\">To Freelancers</a></body>");
            return;
        }
    }
    else
    {
        socket_write_string(clientSocket,"<body>Wrong id<br>"
                                  "<a href=\"/teacher/\">To Freelancers</a></body>");
        return;
    }

    //teacher_set(freelanser[id],NULL,NULL,0,NULL,0);
    teacher_set(freelanser[id],"","",0,0,"");

    server_sent(clientSocket, "<body>Delete success<br>"
                                  "<a href=\"/teacher/\">To Teachers</a></body>");
}

void server_POST(http_request_t req,socket_t * clientSocket,teacher_t** freelanser)
{
    const char * name= http_request_getArg(&req,"name");
    const char * surname= http_request_getArg(&req,"surname");
    const char * age= http_request_getArg(&req,"age");
    const char * date= http_request_getArg(&req,"date");
    const char * salary= http_request_getArg(&req,"salary");

    if(name==NULL || surname==NULL || age ==NULL || date==NULL || salary==NULL)
    {
        socket_write_string(clientSocket,"<body> POST argument/arguments is/are empty<br>"
                                  "<a href=""/teacher/"">To Teachers</a></body>");
        return;
    }

    char buf[1000]="";
    int id;

    if(strpbrk(req.uri,"-0123456789"))
    {
        id=atoi(strpbrk(req.uri,"-0123456789"));
        if(id>9 || id<0)
        {
            socket_write_string(clientSocket,"<body>Id not found<br>"
                                  "<a href=""/teacher/"">To Teachers</a></body>");
            return;
        }
    }
    else
    {
        socket_write_string(clientSocket,"<body>Wrong id<br>"
                                  "<a href=""/teacher/"">To Teachers</a></body>");
        return;
    }

    int check=0;
    int dot=0;

    for(int i=0; i<strlen(age); i++)
        if(!isdigit(age[i]))
        {
            if(age[i]=='.'||dot==0)
                dot++;
            else
                check++;
        }

    for(int i=0; i<strlen(salary); i++)
        if(!isdigit(salary[i]))
            check++;

    for(int i=0; i<4; i++)
        if(!isdigit(date[i]))
            check++;

    if(date[4]!='-')
        check++;

    for(int i=5; i<7; i++)
        if(!isdigit(date[i]))
            check++;

    if(date[7]!='-')
        check++;

    for(int i=8; i<strlen(date); i++)
        if(!isdigit(date[i]))
            check++;

    if(atoi(date+5)>12)
        check++;

    if(dot>1||dot<0)
    {
        socket_write_string(clientSocket,"<body>Invalid POST argument<br>"
                                  "<a href=""/teacher/"">To Teachers</a></body>");
        return;
    }

    if(check==0)
    {
        //teacher_set(freelanser[id],name,surname,atof(age),date,atoi(salary));
        teacher_set(freelanser[id],name,surname,atof(age),atoi(salary),date);

        server_sent(clientSocket, "<body>Post success<br>"
                                  "<a href=\"/teacher/\">To Freelancers</a></body>");
    }
    else
        socket_write_string(clientSocket,"<body>Invalid POST argument<br>"
                                  "<a href=\"/teacher/\">To Freelancers</a></body>");
}

void server_start_page(socket_t* clientSocket)
{
    char text[]="<html>"
                "<head>"
                "<title>Page Title</title>"
                "</head>"
                "<body>"
                "<h1>HELLO Word</h1>"
                "<p><a href=""/teacher/"">Freelancers</a></p>"
                "</body>"
                "</html>";
    socket_write_string(clientSocket,text);
}

void server_GET_html(socket_t* clientSocket,teacher_t ** freelancer)
{
    char buf[10000]="<html>"
                    "<head>"
                    "<title>Page Title</title>"
                    "</head>"
                    "<body>"
                    "<h1>Freelancers</h1>";
    char text [1000]="";
    for(int i=0; i<10; i++)
    {
        if(strcmp(teacher_name_get(freelancer[i]),"")!=0)
        {
            sprintf(text,"<p><a href=""/teacher/%i"">%s %s</a></p>",i,teacher_name_get(freelancer[i]),teacher_surname_get(freelancer[i]));
            strcat(buf,text);
        }
    }
    strcat(buf,"<p><a href=""/teacher/paste"">New teacher</a></p>");
    strcat(buf,"</body>"
           "</html>");
    socket_write_string(clientSocket,buf);
}

void server_GET_id_html(http_request_t req,socket_t* clientSocket,teacher_t ** freelancer)
{
    int id;

    if(strpbrk(req.uri,"-0123456789"))
    {
        id=atoi(strpbrk(req.uri,"-0123456789"));
        if(id>9 || id<0)
        {
            socket_write_string(clientSocket,"Id not found");
            return;
        }
    }
    else
    {
        socket_write_string(clientSocket,"Wrong id");
        return;
    }
    char text [1000]="";
    char buf[10000]="<html>"
                    "<head>"
                    "<title>Page Title</title>"
                    "</head>"
                    "<body>"
                    "<h1>teacher</h1>";


    sprintf(text,"<p>Name:\t\t%s</p>"
            "<p>Surname:\t%s</p>"
            "<p>Age:\t\t%.2f</p>"
            "<p>Date:\t\t%s</p>"
            "<p>\tSalary:\t\t%i\n\n\n</p>"
            "<p><a href=""/teacher/"">To prev page</a></p>",
            teacher_name_get(freelancer[id]),
            teacher_surname_get(freelancer[id]),
            teacher_age_get(freelancer[id]),
            //teacher_date_get(freelancer[id]),
            teacher_salary_get(freelancer[id])
           );
    strcat(buf,text);
    sprintf(text,"<p><a href=\"/teacher/\" onclick=\"doDelete()\">Delete teacher</a></p>"
                 "<script>"
                 "function doDelete(){"
                 "var xhttp=new XMLHttpRequest();"
                 "xhttp.open(\"DELETE\",\"/teacher/api/%i\",true);"
                 "xhttp.send();"
                 "}"
                 "</script>",id);
    strcat(buf,text);
    strcat(buf,"</body>"
           "</html>");
    socket_write_string(clientSocket,buf);
}

void server_Paste_html(http_request_t req,socket_t* clientSocket,teacher_t ** freelancer)
{
    char buf[5000]="";
    char text[5000]="";
    int id=0;
    while(strcmp(teacher_name_get(freelancer[id]),"")!=0)
    {
        id++;
        if(id>9)
        {
            strcat(buf,"<body>"
                        "Can't Post the new Freelancer<br>"
                        "<a href=""/teacher/"">Back</a>"
                        "</body>");
        }
    }
    sprintf(text, "<html>"
            "<body>"
            "<form action=""http://127.0.0.1:5000/teacher/api/%i"" method=""POST"">"
            "Name:<br>"
            "<input type=""text"" name=""name""><br>"
            "Surname:<br>"
            "<input type=""text"" name=""surname"" ><br>"
            "Age:<br>"
            "<input type=""text"" name=""age""><br>"
            "Date:<br>"
            "<input type=""text"" name=""date""><br>"
            "Salary:<br>"
            "<input type=""text"" name=""salary""><br>"
            "<input type=""submit"" value='Send POST request' />"
            "</form>"
            "</body>",id);
    strcat(buf,text);
    socket_write_string(clientSocket,buf);
}


http_request_t http_request_parse(const char * const request)
{
    http_request_t req;
    req.form = NULL;
    req.formLength = 0;
    // get method
    ptrdiff_t methodLen = strstr(request, " ") - request;  // find first whitespace
    memcpy(req.method, request, methodLen);
    req.method[methodLen] = '\0';
    // get uri
    const char * uriStartPtr = request + strlen(req.method) + 1;
    const char * uriEndPtr = strstr(uriStartPtr, " ");  // find second whitespace
    ptrdiff_t uriLen = uriEndPtr - uriStartPtr;
    memcpy(req.uri, uriStartPtr, uriLen);
    req.uri[uriLen] = '\0';
    // parse form data
    const char * bodyStartPtr = strstr(request, "\r\n\r\n") + strlen("\r\n\r\n");
    const char * cur = bodyStartPtr;
    const char * pairEndPtr = cur;
    const char * eqPtr = cur;
    while (strlen(cur) > 0)
    {
        pairEndPtr = strchr(cur, '&');
        if (NULL == pairEndPtr)
        {
            pairEndPtr = cur + strlen(cur);
        }
        keyvalue_t kv;
        // get key
        eqPtr = strchr(cur, '=');
        ptrdiff_t keyLen = eqPtr - cur;
        memcpy(kv.key, cur, keyLen);
        kv.key[keyLen] = '\0';
        // get value
        eqPtr++;
        ptrdiff_t valueLen = pairEndPtr - eqPtr;
        memcpy(kv.value, eqPtr, valueLen);
        kv.value[valueLen] = '\0';
        // insert key-value pair into request form list
        req.formLength += 1;
        req.form = realloc(req.form, sizeof(keyvalue_t) * req.formLength);
        req.form[req.formLength - 1] = kv;
        cur = pairEndPtr + ((strlen(pairEndPtr) > 0) ? 1 : 0);
    }
    return req;
}

const char * http_request_getArg(http_request_t * self, const char * key)
{
    for (int i = 0; i < self->formLength; i++)
    {
        if (strcmp(self->form[i].key, key) == 0)
        {
            return self->form[i].value;
        }
    }
    return NULL;
}

const char * keyvalue_toString(keyvalue_t * self)
{
    char * str = malloc(sizeof(char) * (strlen(self->key) + strlen(self->value) + 2));
    sprintf(str, "%s=%s\0", self->key, self->value);
    return str;
}


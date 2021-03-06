#include <yami4-cpp/yami.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "difhw.h"
uint32_t gtc[255];
static uint32_t br=0;
void update(yami::incoming_message & message)
{
    const yami::parameters & content =
        message.get_parameters();

    Difhw::Data d;
    d.read(content);


    char* buf=d.Payload.data();int* ibuf=(int*) buf;
    gtc[0]++;
    gtc[d.Difid]=d.Gtc;
    br+=d.Payload.size();
    if (gtc[0]%1000==0)
      {
	for (int i=1;i<=254;i++)
	  if (gtc[i]!=0) printf("%d %d \n",i,gtc[i]);
	printf ("Br %f \n",br/1024./1024.);
      }
    //  std::cout << "received update: " << d.Difid<<" "<<d.Gtc <<" "<<d.Payload.size()<<"->"<<ibuf[0]<<" "<<ibuf[1]<<" "<<ibuf[2]<< std::endl;
    
}

int main(int argc, char * argv[])
{
  memset(gtc,0,255*4);
    if (argc != 2)
    {
        std::cout << "expecting three parameters: "
            "server destination \n";
        return EXIT_FAILURE;
    }

    try
    {
        yami::agent client_agent;

#ifdef NONAMESRV
    const std::string server_address = argv[1];
#else
    const std::string name_server_address = argv[1];

    yami::parameters list_params;

    list_params.set_string("object", "unessai");
    

    std::auto_ptr<yami::outgoing_message> ns_query1(
						   client_agent.send(name_server_address,
								     "names", "list", list_params));

    ns_query1->wait_for_completion();

        if (ns_query1->get_state() != yami::replied)
        {
            std::cout << "error: "
                << ns_query1->get_exception_msg() << std::endl;

            return EXIT_FAILURE;
        }

        const yami::parameters & list_reply =
	  ns_query1->get_reply();


    

      std::vector<std::string> vnames;
      std::vector<std::string> vlocs;
      
        std::size_t size_ = list_reply.get_string_array_length("location");
        vnames.resize(size_);
        vlocs.resize(size_);
        for (std::size_t i_ = 0; i_ != size_; ++i_)
        {
            vnames[i_] = list_reply.get_string_in_array("object", i_);
            vlocs[i_] = list_reply.get_string_in_array("location", i_);
	    std::cout<<vnames[i_]<<"@"<<vlocs[i_]<<std::endl;
        }



    std::string dummyl;
    std::cin >> dummyl;



    // Un seul
    yami::parameters resolve_params;

    resolve_params.set_string("object", "#lyopc252#DIF");

    std::auto_ptr<yami::outgoing_message> ns_query(
						   client_agent.send(name_server_address,
								     "names", "resolve", resolve_params));
        
        ns_query->wait_for_completion();
        if (ns_query->get_state() != yami::replied)
        {
            std::cout << "error: "
                << ns_query->get_exception_msg() << std::endl;

            return EXIT_FAILURE;
        }

        const yami::parameters & resolve_reply =
	  ns_query->get_reply();
        const std::string & server_address =
            resolve_reply.get_string("location");

#endif



	yami::parameters params;
	const std::string update_object_name =
            "update_handler";
        params.set_string("destination_object", update_object_name);
	client_agent.register_object(update_object_name, update);
	Difhw::Statemachine s(client_agent,server_address,"#lyopc252#DIF");
	Difhw::Scanstatus Res;
	Difhw::Config cf;
	Difhw::Difstatus dst;

	
	s.Stop(dst);
	s.Scan(Res);
	for (std::vector<int>::iterator it=Res.Diflist.begin();it!=Res.Diflist.end();it++)
	  std::cout<<(*it)<<std::endl;




        






	s.Initialise(Res,dst);
	for (int i=0;i<dst.Status.size();i++)
	  std::cout<<std::hex<<dst.Status[i]<<std::dec<<" "<<dst.Debug[i]<<std::endl;

	cf.Trigger=0xb00;
	cf.Dbstate="Mon_EtaT_amoi";
	s.Registerdb(cf,dst);
	for (int i=0;i<dst.Status.size();i++)
	    std::cout<<std::hex<<dst.Status[i]<<std::dec<<" "<<dst.Debug[i]<<std::endl;
       
	


	std::string dummy;
	std::cin >> dummy;

	s.Start(dst);
	for (int i=0;i<dst.Status.size();i++)
	  std::cout<<std::hex<<dst.Status[i]<<std::dec<<" "<<dst.Debug[i]<<std::endl;


// subscribe to the producer

	/*
	for (std::vector<int>::iterator it=Res.Diflist.begin();it!=Res.Diflist.end();it++)
	  {
	    std::cout<<"Subscirbing " <<(*it)<<std::endl;
	    std::stringstream ss;
	    ss<<"/DIFSERVER/DIF"<<(*it)<<"/DATA";
	*/
	for (int i=1;i<255;i++)
	  {
	     std::cout<<"Subscirbing " <<i<<std::endl;
	    std::stringstream ss;
	    ss<<"/DIFSERVER/DIF"<<i<<"/DATA";
	    client_agent.send_one_way(server_address,
				      ss.str(), "subscribe", params);
	  }
	std::cin >> dummy;
	//s.Stop(dst);
	for (int i=0;i<dst.Status.size();i++)
	  std::cout<<std::hex<<dst.Status[i]<<std::dec<<" "<<dst.Debug[i]<<std::endl;

	
    }
    catch (const std::exception & e)
    {
        std::cout << "error: " << e.what() << std::endl;
    }
}

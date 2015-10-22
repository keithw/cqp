#ifndef MACHINES_HH
#define MACHINES_HH

#include <string>
#include <array>

struct Machine
{
  std::string type, best_client, family, pessimistic_family, nice_name;

  Machine( const std::string & type, const std::string & best_client,
	   const std::string & family, const std::string & pessimistic_family,
	   const std::string & nice_name )
    : type( type ), best_client( best_client ),
      family( family ), pessimistic_family( pessimistic_family ),
      nice_name( nice_name )
  {}
};

static const std::array<Machine, 9> DrCloudMachines = {
  Machine( "i2.1x", "i2.8x", "machines_aws.csv", "machines_aws_advertised.csv", "EC2 i2.1xlarge" ),
  Machine( "i2.2x", "i2.8x", "machines_aws.csv", "machines_aws_advertised.csv", "EC2 i2.2xlarge" ),
  Machine( "i2.4x", "i2.8x", "machines_aws.csv", "machines_aws_advertised.csv", "EC2 i2.4xlarge" ),
  Machine( "i2.8x", "i2.8x", "machines_aws.csv", "machines_aws_advertised.csv", "EC2 i2.8xlarge" ),
  Machine( "n1-highmem-2", "n1-highmem-32", "machines_gcloud.csv", "machines_gcloud_min.csv", "Google n1-highmem-2" ),
  Machine( "n1-highmem-4", "n1-highmem-32", "machines_gcloud.csv", "machines_gcloud_min.csv", "Google n1-highmem-4" ),
  Machine( "n1-highmem-8", "n1-highmem-32", "machines_gcloud.csv", "machines_gcloud_min.csv", "Google n1-highmem-8" ),
  Machine( "n1-highmem-16", "n1-highmem-32", "machines_gcloud.csv", "machines_gcloud_min.csv", "Google n1-highmem-16" ),
  Machine( "n1-highmem-32", "n1-highmem-32", "machines_gcloud.csv", "machines_gcloud_min.csv", "Google n1-highmem-32" )
};

#endif /* MACHINES_HH */

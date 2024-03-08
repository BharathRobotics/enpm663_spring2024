#pragma once

#include <chrono>
#include <interface_demo_msgs/msg/vehicle_status.hpp>
#include <interface_demo_msgs/srv/get_speed_profile.hpp>
#include <iostream>
#include <memory>
#include <rclcpp/rclcpp.hpp>

// aliases
using GetSpeedProfileSrv = interface_demo_msgs::srv::GetSpeedProfile;
using VehicleStatusMsg = interface_demo_msgs::msg::VehicleStatus;
/**
 * @brief Class for the client
 *
 * This class demonstrates how to send a request to a server using clients.
 * The clients are used for both synchronous and asynchronous communication.
 * The clients are called in the subscriber callback function.
 */
class ServiceClientSubscriberInterface : public rclcpp::Node {
 public:
  ServiceClientSubscriberInterface(std::string node_name) : Node(node_name) {
    // create a callback group for the synchronous client
    // this is needed to ensure that the client does not block the subscriber
    mutex_group_ = this->create_callback_group(
        rclcpp::CallbackGroupType::MutuallyExclusive);

    // initialize the subscriber object
    subscriber_ = this->create_subscription<VehicleStatusMsg>(
        "vehicle_status", 10,
        std::bind(&ServiceClientSubscriberInterface::subscriber_callback, this,
                  std::placeholders::_1));

    // initialize the client object for asynchronous communication
    async_client_ =
        this->create_client<GetSpeedProfileSrv>("get_speed_profile");

    // initialize the client object for synchronous communication
    sync_client_ = this->create_client<GetSpeedProfileSrv>(
        "get_speed_profile",               // service name
        rmw_qos_profile_services_default,  // qos profile
        mutex_group_                       // callback group
    );

    request_ = std::make_shared<GetSpeedProfileSrv::Request>();
  }

 private:
  std::shared_ptr<interface_demo_msgs::srv::GetSpeedProfile_Request> request_;
  // callback group for the synchronous client
  rclcpp::CallbackGroup::SharedPtr mutex_group_;
  // client object for asynchronous communication
  rclcpp::Client<GetSpeedProfileSrv>::SharedPtr async_client_;
  // client object for synchronous communication
  rclcpp::Client<GetSpeedProfileSrv>::SharedPtr sync_client_;
  // subscriber object to interface_demo_msgs::msg::VehicleStatus
  rclcpp::Subscription<VehicleStatusMsg>::SharedPtr subscriber_;

  /**
   * @brief A function to print the speed profile based on the response
   * from the server
   * @param result Response from the server
   */
  void print_profile(int result);

  /**
   * @brief Callback function for the subscriber on the topic "vehicle_status"
   * @param msg Shared pointer to the message object
   */
  void subscriber_callback(const VehicleStatusMsg::SharedPtr msg);

  /**
   * @brief Send a request to the server asynchronously
   * @param speed Speed of the vehicle
   */
  void send_async_request(double speed);

  /**
   * @brief Send a request to the server synchronously
   * @param speed Speed of the vehicle
   */
  void send_sync_request(double speed);

  /**
   * @brief Callback function for the client
   *
   * This function is called when the client receives a response from the server
   * @param future Shared pointer to the future object.
   *  A future is a value that indicates whether the call and response is
   * finished (not the value of the response itself)
   */
  void response_callback(
      rclcpp::Client<GetSpeedProfileSrv>::SharedFuture future);
};
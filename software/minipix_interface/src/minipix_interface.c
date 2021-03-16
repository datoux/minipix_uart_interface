#include <minipix_interface.h>

/* mui_initialize() //{ */

void mui_initialize(MUI_Handler_t* mui_handler) {

  // initialize the inner state of the LLCP receiver
  llcp_initialize(&mui_handler->llcp_receiver);

  printf("[MinipixInterface]: initialized\n");
}

//}

/* mui_measureFrame() //{ */

void mui_measureFrame(MUI_Handler_t* mui_handler, const uint16_t acquisition_time) {

  MeasureFrameReqMsg_t get_frame;
  get_frame.message_id                  = LLCP_MEASURE_FRAME_MSG_ID;
  get_frame.payload.acquisition_time_ms = acquisition_time;
  hton_MeasureFrameReqMsg_t(&get_frame);

  printf("asking for acquisition %d\n", acquisition_time);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&get_frame, sizeof(get_frame), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

/* mui_getStatus() //{ */

void mui_getStatus(MUI_Handler_t* mui_handler) {

  GetStatusMsg_t get_status;
  get_status.message_id = LLCP_GET_STATUS_MSG_ID;
  hton_GetStatusMsg_t(&get_status);

  uint16_t n_bytes = llcp_prepareMessage((uint8_t*)&get_status, sizeof(get_status), mui_handler->tx_buffer);

  mui_handler->fcns.sendString(mui_handler->tx_buffer, n_bytes);
}

//}

// | ------------- UART communication with MiniPIX ------------ |

/* mui_receiveCharCallback() //{ */

void mui_receiveCharCallback(MUI_Handler_t* mui_handler, const uint8_t byte_in) {

  LLCP_Message_t message_in;

  if (llcp_processChar(byte_in, &mui_handler->llcp_receiver, &message_in)) {

    switch ((LLCPMessageId_t)message_in.id) {

      case LLCP_IMAGE_DATA_MSG_ID: {

        ImageDataMsg_t* msg = (ImageDataMsg_t*)&message_in.payload;
        ntoh_ImageDataMsg_t(msg);

        mui_handler->fcns.processImagePacket(&msg->payload);

        break;
      };

      case LLCP_STATUS_MSG_ID: {

        StatusMsg_t* msg = (StatusMsg_t*)&message_in.payload;
        ntoh_StatusMsg_t(msg);

        mui_handler->fcns.processStatus(&msg->payload);

        break;
      };

      default: {

        printf("Received unsupported message with id = %d\n", message_in.id);
      }
    }
  }
}

//}

// | -------------------- LED signalization ------------------- |

/* mui_ledSet() //{ */

void mui_ledSet(MUI_Handler_t* mui_handler, const bool new_state) {

  mui_handler->fcns.ledSetHW(new_state);

  mui_handler->led_state = new_state;
}

//}

/* mui_ledToggle() //{ */

void mui_ledToggle(MUI_Handler_t* mui_handler) {

  mui_ledSet(mui_handler, !mui_handler->led_state);
}

//}

// | --------------- the main update() function --------------- |

/* mui_update() //{ */

void mui_update(MUI_Handler_t* mui_handler) {

  // TODO process chars from the buffer

  mui_ledToggle(mui_handler);
  mui_handler->fcns.sleepHW((uint16_t)100);
}

//}

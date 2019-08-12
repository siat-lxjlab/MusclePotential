/**
 * Copyright (c) 2012 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup ble_hrs Heart Rate Service
 * @{
 * @ingroup ble_sdk_srv
 * @brief Heart Rate Service module.
 *
 * @details This module implements the Heart Rate Service with the Heart Rate Measurement,
 *          Body Sensor Location and Heart Rate Control Point characteristics.
 *          During initialization it adds the Heart Rate Service and Heart Rate Measurement
 *          characteristic to the BLE stack database. Optionally it also adds the
 *          Body Sensor Location and Heart Rate Control Point characteristics.
 *
 *          If enabled, notification of the Heart Rate Measurement characteristic is performed
 *          when the application calls ble_hrs_heart_rate_measurement_send().
 *
 *          The Heart Rate Service also provides a set of functions for manipulating the
 *          various fields in the Heart Rate Measurement characteristic, as well as setting
 *          the Body Sensor Location characteristic value.
 *
 *          If an event handler is supplied by the application, the Heart Rate Service will
 *          generate Heart Rate Service events to the application.
 *
 * @note    The application must register this module as BLE event observer using the
 *          NRF_SDH_BLE_OBSERVER macro. Example:
 *          @code
 *              ble_hrs_t instance;
 *              NRF_SDH_BLE_OBSERVER(anything, BLE_HRS_BLE_OBSERVER_PRIO,
 *                                   ble_hrs_on_ble_evt, &instance);
 *          @endcode
 *
 * @note Attention!
 *  To maintain compliance with Nordic Semiconductor ASA Bluetooth profile
 *  qualification listings, this section of source code must not be modified.
 */

#ifndef BLE_HRS_H__
#define BLE_HRS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif


//�������ʷ���ʵ������ʵ�����2������
//1��������static�������ʷ���ṹ�������Ϊ���ʷ���ṹ��������ڴ�
//2��ע����BLE�¼������ߣ���ʹ�����ʳ���ģ����Խ���BLEЭ��ջ���¼����Ӷ�������ble_hrs_on_ble_evt()�¼��ص������д����Լ�����Ȥ���¼�
#define BLE_HRS_DEF(_name)                                                                          \
static ble_hrs_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_hrs_on_ble_evt, &_name)


//���������λ�ñ���
#define BLE_HRS_BODY_SENSOR_LOCATION_OTHER      0 //����λ��
#define BLE_HRS_BODY_SENSOR_LOCATION_CHEST      1 //�ز�
#define BLE_HRS_BODY_SENSOR_LOCATION_WRIST      2 //����
#define BLE_HRS_BODY_SENSOR_LOCATION_FINGER     3 //��ָ
#define BLE_HRS_BODY_SENSOR_LOCATION_HAND       4 //��
#define BLE_HRS_BODY_SENSOR_LOCATION_EAR_LOBE   5 //����
#define BLE_HRS_BODY_SENSOR_LOCATION_FOOT       6 //��

//RR Interval��������
#define BLE_HRS_MAX_BUFFERED_RR_INTERVALS       20     


//���ʷ����¼�����
typedef enum
{
    BLE_HRS_EVT_NOTIFICATION_ENABLED,   //����ֵ֪ͨʹ���¼�
    BLE_HRS_EVT_NOTIFICATION_DISABLED   //����ֵ֪ͨ�ر��¼�
} ble_hrs_evt_type_t;


//���ʷ����¼��ṹ��
typedef struct
{
    //���ʷ����¼�����
	  ble_hrs_evt_type_t evt_type;   
} ble_hrs_evt_t;

//�����������ʷ���ṹ�壬����ble_hrs_s����Ϊble_hrs_t
typedef struct ble_hrs_s ble_hrs_t;

//��������ָ����������ʷ���ṹ�������䶨���˺���ָ�����Ԥ�����û�ʹ��
typedef void (*ble_hrs_evt_handler_t) (ble_hrs_t * p_hrs, ble_hrs_evt_t * p_evt);


//���ʷ����ʼ���ṹ�壬������ʼ�����ʷ������������ѡ�������
typedef struct
{
    //���ʷ����¼��ص�������Ԥ�����û�ʹ�ã�������û��ʹ�ã���ʼ��ʱ����ΪNULL
	  ble_hrs_evt_handler_t        evt_handler;                                          
    //�������Ӵ����֧�ֱ�־
	  bool                         is_sensor_contact_supported;                         
    //������λ��
	  uint8_t *                    p_body_sensor_location;                              
    //дCCCD���ͻ���������������ʱ�İ�ȫ����
	  security_req_t               hrm_cccd_wr_sec;                                      
    //��BSL��������λ�ã�����ֵʱ�İ�ȫ����
	  security_req_t               bsl_rd_sec;                                           
} ble_hrs_init_t;


//���ʷ���ṹ�壬�������ʷ������������ѡ�������
struct ble_hrs_s
{
    //���ʷ����¼��ص�������Ԥ�����û�ʹ�ã�������û��ʹ�ã���ʼ��ʱ����ΪNULL
	  ble_hrs_evt_handler_t        evt_handler;                                         
    //��������֧�ֱ�־��true=֧����������ͳ��
	  bool                         is_expended_energy_supported;                         
    //�������Ӵ����֧�ֱ�־��true=֧�ִ������Ӵ����
	  bool                         is_sensor_contact_supported;                         
    //���ʷ���������Э��ջ�ṩ��
	  uint16_t                     service_handle;                                       
    //���ʲ����������
	  ble_gatts_char_handles_t     hrm_handles;                                         
    //������������λ���������
	  ble_gatts_char_handles_t     bsl_handles;                                          
    //Heart Rate Control Point�������
	  ble_gatts_char_handles_t     hrcp_handles;                                         
    //���Ӿ������Э��ջ�ṩ����δ�������ӣ���ֵΪBLE_CONN_HANDLE_INVALID��
	  uint16_t                     conn_handle;                                          
    //�������Ӵ���־��ture=��⵽�������Ӵ�
	  bool                         is_sensor_contact_detected;                          
    //���ϴ����ʲ��Դ�����һ��rr_interval
	  uint16_t                     rr_interval[BLE_HRS_MAX_BUFFERED_RR_INTERVALS];       
    //���ϴ����ʲ��Դ�����rr_interval����
		uint16_t                     rr_interval_count;                                    
    //��ǰһ�οɷ��͵�������ʲ������ݳ��ȣ����ݵ�ǰATT MTUֵ����
		uint8_t                      max_hrm_len;                                          
};


/**@brief Function for initializing the Heart Rate Service.
 *
 * @param[out]  p_hrs       Heart Rate Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_hrs_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_hrs_init(ble_hrs_t * p_hrs, ble_hrs_init_t const * p_hrs_init);


/**@brief Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the Heart Rate Service.
 *
 * @param[in]   p_hrs      Heart Rate Service structure.
 * @param[in]   p_gatt_evt  Event received from the GATT module.
 */
void ble_hrs_on_gatt_evt(ble_hrs_t * p_hrs, nrf_ble_gatt_evt_t const * p_gatt_evt);


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Heart Rate Service.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 * @param[in]   p_context   Heart Rate Service structure.
 */
void ble_hrs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending heart rate measurement if notification has been enabled.
 *
 * @details The application calls this function after having performed a heart rate measurement.
 *          If notification has been enabled, the heart rate measurement data is encoded and sent to
 *          the client.
 *
 * @param[in]   p_hrs                    Heart Rate Service structure.
 * @param[in]   heart_rate               New heart rate measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_hrs_heart_rate_measurement_send(ble_hrs_t * p_hrs, uint16_t heart_rate);



/**@brief Function for setting the state of the Sensor Contact Detected bit.
 *
 * @param[in]   p_hrs                        Heart Rate Service structure.
 * @param[in]   is_sensor_contact_detected   TRUE if sensor contact is detected, FALSE otherwise.
 */
void ble_hrs_sensor_contact_detected_update(ble_hrs_t * p_hrs, bool is_sensor_contact_detected);




#ifdef __cplusplus
}
#endif

#endif // BLE_HRS_H__

/** @} */

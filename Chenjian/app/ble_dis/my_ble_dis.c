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
/* Attention!
 * To maintain compliance with Nordic Semiconductor ASA's Bluetooth profile
 * qualification listings, this section of source code must not be modified.
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_MY_DIS)
#include "my_ble_dis.h"

#include <stdlib.h>
#include <string.h>
#include "app_error.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"




//����uint16���ͱ����������������������ӷ���ɹ���Э��ջ����������浽�ñ�����֮��þ������������־�豸��Ϣ����
static uint16_t                 service_handle;
static ble_gatts_char_handles_t manufact_name_handles;


/**@brief Function for adding the Characteristic.
 *
 * @param[in]   uuid           UUID of characteristic to be added.
 * @param[in]   p_char_value   Initial value of characteristic to be added.
 * @param[in]   char_len       Length of initial value. This will also be the maximum value.
 * @param[in]   rd_sec         Security requirement for reading characteristic value.
 * @param[out]  p_handles      Handles of new characteristic.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
//��������������ַ�������
static uint32_t char_add(uint16_t                        uuid,
                         uint8_t                       * p_char_value,
                         uint16_t                        char_len,
                         security_req_t const            rd_sec,
                         ble_gatts_char_handles_t      * p_handles)
{
    //�������������ṹ�����
	  ble_add_char_params_t add_char_params;
    //���ָ�������������ַ�����ָ���Ƿ���Ч
    APP_ERROR_CHECK_BOOL(p_char_value != NULL);
	  //��������������ַ��Ƿ����0
    APP_ERROR_CHECK_BOOL(char_len > 0);
    //��ʼ������֮ǰ��������add_char_params
    memset(&add_char_params, 0, sizeof(add_char_params));

    //�����������ַ���������UUID
	  add_char_params.uuid            = uuid;
	  //���������������ַ�������ֵ����󳤶�
    add_char_params.max_len         = char_len;
	  //���������������ַ�������ֵ�ĳ�ʼ����
    add_char_params.init_len        = char_len;
	  //���������������ַ��������ĳ�ʼֵ
    add_char_params.p_init_value    = p_char_value;
	  //���������������ַ������������ʣ�֧�ֶ�ȡ
    add_char_params.char_props.read = 1;
    add_char_params.read_access     = rd_sec;
    //���豸��Ϣ��������������������ַ�������
    return characteristic_add(service_handle, &add_char_params, p_handles);
}

//��ʼ���豸��Ϣ����
uint32_t ble_dis_init(ble_dis_init_t const * p_dis_init)
{
    uint32_t   err_code;
	  //����UUID�ṹ�����
    ble_uuid_t ble_uuid;

    //�豸��Ϣ�����UUID
    BLE_UUID_BLE_ASSIGN(ble_uuid, BLE_UUID_DEVICE_INFORMATION_SERVICE);
    //��ӷ������Ա�
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &service_handle);
    //��鷵�صĴ������
	  if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    //��������������ַ������ȴ���0����ʾ�ַ�����Ч
    if (p_dis_init->manufact_name_str.length > 0)
    {
        //��������������ַ�������
			  err_code = char_add(BLE_UUID_MANUFACTURER_NAME_STRING_CHAR,
                            p_dis_init->manufact_name_str.p_str,
                            p_dis_init->manufact_name_str.length,
                            p_dis_init->dis_char_rd_sec,
                            &manufact_name_handles);
        //��鷵�صĴ������
			  if (err_code != NRF_SUCCESS)
        {
            return err_code;
        }
    }
    
    return NRF_SUCCESS;
}
#endif // NRF_MODULE_ENABLED(BLE_DIS)

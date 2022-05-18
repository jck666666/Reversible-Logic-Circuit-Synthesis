cmake_minimum_required(VERSION 3.22)
project(circuit)

set(CMAKE_CXX_STANDARD 14)

add_executable(circuit
        GNQTS/GNQTS_gw/GNQTS_gw_data_ver_�p��un.cpp
        GNQTS/GNQTS_gw/GNQTS_gw_data_ver_�p��unbm.cpp
        GNQTS/GNQTS_gw/GNQTS_gw_data_ver_�p��unmm.cpp
        GNQTS/GNQTS_gw/GNQTS_gw_test_m_0.25unmm.cpp
        GNQTS/GNQTS_lw/GNQTS_data_ver_�p��un.cpp
        GNQTS/GNQTS_lw/GNQTS_data_ver_�p��unbm.cpp
        GNQTS/GNQTS_lw/GNQTS_data_ver_�p��unmm.cpp
        GNQTS/previous/GNQTS_0.25nu.cpp
        GNQTS/previous/GNQTS_final.cpp
        KNQTS/KNQTS.cpp
        KNQTS/KNQTS_change_hamming_distance.cpp
        KNQTS/KNQTS_lw_ver.cpp
        KNQTS/KNQTS_���p�ߧ�쥿�Ѫ���.cpp
        KNQTS/KNQTS_�p��un.cpp
        KNQTS_gw_ver/KNQTS_gw_data_ver_0.25unmm.cpp
        KNQTS_gw_ver/KNQTS_gw_data_ver_�p��un.cpp
        KNQTS_gw_ver/KNQTS_gw_data_ver_�p��unbm.cpp
        KNQTS_gw_ver/KNQTS_gw_data_ver_�p��unmm.cpp
        KNQTS_gw_ver/KNQTS_gw_test_m_�p��un.cpp
        KNQTS_gw_ver/KNQTS_gw_test_m_�p��unbm.cpp
        KNQTS_gw_ver/KNQTS_gw_test_m_�p��unmm.cpp
        KNQTS_lw_ver/KNQTS_data_ver_�p��un.cpp
        KNQTS_lw_ver/KNQTS_data_ver_�p��unbm.cpp
        KNQTS_lw_ver/KNQTS_data_ver_�p��unmm.cpp
        KNQTS_lw_ver/KNQTS_test_m_�p��un.cpp
        KNQTS_lw_ver/KNQTS_test_m_�p��unbm.cpp
        KNQTS_lw_ver/KNQTS_test_m_�p��unmm.cpp
        QTS/circuit_alignment_50Exp_mem_mantain_GNQTS_ver.cpp
        QTS/circuit_alignment_50Exp_mem_mantain_ver.cpp
        QTS/circuit_alignment_PPTver.cpp
        QTS/circuit_alignment_random_ver.cpp
        QTS/circuit_alignment_ver.cpp
        brutal.cpp
        KNQTS_gw_0.25unmm_test_for_new_function.cpp
        KNQTS_gw_test_m_0.25unmm.cpp
        �ɤO��.cpp)

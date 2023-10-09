<?php
    class Regul_Airtel_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert($data) {
            return $this->db->insert("regul_airtel", $data);
        }

        public function insert_vi_deallo ($data) {
            return $this->db->insert("regul_vi_deallo", $data);
        }

        public function insert_igor_airtel($data) {
            return $this->db->insert("regul_igor_airtel", $data);
        }

        public function get_regul_ci() {

            $this->db->select('boa_airtel_nonau.*, igor_airtel_anomalie_ci.*');
            $this->db->from('boa_airtel_nonau');
            $this->db->join('igor_airtel_anomalie_ci', 'boa_airtel_nonau.ref_igor = igor_airtel_anomalie_ci.REF_IGOR');
            
            $query = $this->db->get();
            return $query->result();
        }

        public function get_regul_deallo() {
            $this->db->select('igor_airtel_anomalie_vi.*, airtel_deallocation.*');
            $this->db->from('igor_airtel_anomalie_vi');
            $this->db->join('airtel_deallocation', 'airtel_deallocation.REF_IGOR = igor_airtel_anomalie_vi.REF_IGOR');
            $query = $this->db->get();
            return $query->result(); 
        }

        public function get_ambi() {
            $this->db->select('airtel_rollback.*, airtel_ambiguous.*');
            $this->db->from('airtel_ambiguous');
            $this->db->join('airtel_rollback', 'airtel_ambiguous.TRANSFER_ID = airtel_rollback.rollback_reference_number', 'INNER');
            $query = $this->db->get();
            return $query->result();

        }
    }
?>
<?php
    class Regul_Telma_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }


        public function get_regul_ci() {
            $this->db->select('boa_telma_nonau.*, boa_telma_anomalie_ci.*');
            $this->db->from('boa_telma_nonau');
            $this->db->join('boa_telma_anomalie_ci', 'boa_telma_nonau.ref_igor = boa_telma_anomalie_ci.REF_IGOR');
            $query = $this->db->get();
            return $query->result();
        }

        

    }
?>
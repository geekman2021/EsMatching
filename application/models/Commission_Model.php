<?php 
    class Commission_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update($data) {

            $this->db->where("com_ref_igor", $data["com_ref_igor"]);
            $exist= $this->db->get("commission");
            if($exist->num_rows() == 0) {
                return $this->db->insert("commission", $data);
            } else {
                $this->db->where("com_ref_igor", $data["com_ref_igor"]);
                return $this->db->update("commission", $data);
            }
        }



        
    }
?>
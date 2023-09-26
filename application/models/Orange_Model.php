<?php 
    class Orange_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("db1");
        }

        public function insert_or_update($data) {

            $this->db->where("reference", $data["reference"]);
            $exist= $this->db->get("orange");
            if($exist->num_rows() == 0) {
                return $this->db->insert("orange", $data);
            } else {
                $this->db->where("reference", $data["reference"]);
                return $this->db->update("orange", $data);
            }
        }

        public function get_normal_CashI() {
            
            
        }

        public function get_normal_CashO() {

           

        }

        public function insert_anormale_CO() {

         
        }
        public function get_anormale_CO() {
        
        }

        public function get_anormale_CI() {

           

        }

        public function insererAnomalieCI() {

        }

        public function update_anormale_CO($id, $etat, $date_regul) {

        }

        public function update_anormale_CI($id, $etat, $date_regul) {

        }

        
    }
?>
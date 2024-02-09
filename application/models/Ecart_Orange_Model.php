<?php 
    class Ecart_Orange_Model extends CI_Model {

        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function get_ecart_orange() {
            $sql = "SELECT * FROM orange_ind 
                        UNION 
                    SELECT * FROM orange_anomalie_co 
                        UNION 
                    SELECT * FROM orange_anomalie_ci";
    
            $query = $this->db->query($sql);
            return $query->result();
        }

        public function get_ecart_boa() {
            $sql= "SELECT comm_compte, comm_date_oper, comm_date_val, comm_devise, comm_montant, comm_libelle, comm_oper, comm_expl, comm_ref_igor, comm_ref_rel, comm_code_agence, princ_compte, princ_date_oper, princ_date_val, princ_devise, princ_montant, princ_libelle, princ_oper, princ_expl, princ_ref_igor, princ_ref_rel, cle FROM boa_orange_anomalie_ci     
                        UNION ALL
                SELECT  NULL AS comm_compte, NULL AS comm_date_oper, NULL AS comm_date_val, NULL AS comm_devise, NULL AS comm_montant, NULL AS comm_libelle,  NULL AS comm_oper, NULL AS comm_expl, NULL AS comm_ref_igor, NULL AS comm_ref_rel, NULL AS comm_code_agence, princ_compte, princ_date_oper, princ_date_val, princ_devise, princ_montant, princ_libelle, princ_oper, princ_expl, princ_ref_igor, princ_ref_rel, cle FROM boa_orange_anomalie_co
                        UNION ALL
                SELECT NULL AS comm_compte, NULL AS comm_date_oper, NULL AS comm_date_val, NULL AS comm_devise, NULL AS comm_montant, NULL AS comm_libelle,  NULL AS comm_oper, NULL AS comm_expl, NULL AS comm_ref_igor, NULL AS comm_ref_rel, NULL AS comm_code_agence, princ_compte, princ_date_oper, princ_date_val, princ_devise, princ_montant, princ_libelle, princ_oper, princ_expl, princ_ref_igor, princ_ref_rel, NULL as cle FROM boa_orange_vi";
            
            $query= $this->db->query($sql);


            return $query->result();
        }
    }
?>
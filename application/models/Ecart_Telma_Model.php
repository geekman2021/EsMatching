<?php
    class Ecart_Telma_Model extends CI_Model {
        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }


        public function get_ecart_telma() {
            $sql = "SELECT date_d, date, trans_id, initiator, TYPE, channel, state, wallet ,Amount_MGA, solde, sender, sender_name, receiver, receiver_name, details1, confirming_agent, notify_alternate, origine, TVA, ACTION, AA1_GROUP, PAR FROM telma_mvts_ci 
                            UNION 
                    SELECT date_d, date, trans_id, initiator, TYPE, channel, state, wallet ,Amount_MGA, solde, sender, sender_name, receiver, receiver_name, details1, confirming_agent, notify_alternate, origine, TVA, ACTION, AA1_GROUP, PAR  FROM telma_mvts_co
                            UNION 
                    SELECT date_d, date, trans_id, initiator, TYPE, channel, state, wallet ,Amount_MGA, solde, sender, sender_name, receiver, receiver_name, details1, confirming_agent, notify_alternate, origine, TVA, ACTION, AA1_GROUP, PAR  FROM telma_admin WHERE NOT EXISTS (SELECT * FROM boa_telma_anomalie_vi WHERE boa_telma_anomalie_vi.trans_id = telma_admin.trans_id)";
            $query= $this->db->query($sql);

            return $query->result();
        }

        public function get_ecart_boa() {
            $sql= "SELECT boa_telma_anomalie_ci.COMPTE, boa_telma_anomalie_ci.DATE_OPER, boa_telma_anomalie_ci.DATE_VAL, boa_telma_anomalie_ci.DEVISE, boa_telma_anomalie_ci.MONTANT, boa_telma_anomalie_ci.LIBELLE, boa_telma_anomalie_ci.OPER, boa_telma_anomalie_ci.EXPL, boa_telma_anomalie_ci.REF_IGOR   FROM boa_telma_anomalie_ci LEFT JOIN boa_telma_nonau ON boa_telma_anomalie_ci.REF_IGOR = boa_telma_nonau.ref_igor WHERE boa_telma_nonau.ref_igor IS NULL
                            UNION 
                    SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR FROM boa_telma_anomalie_co 
                            UNION 
                    SELECT COMPTE, DATE_OPER, DATE_VAL, DEVISE, MONTANT, LIBELLE, OPER, EXPL, REF_IGOR FROM boa_telma_anomalie_vi WHERE trans_id IS NULL";
            
            $query= $this->db->query($sql);

            return $query->result();
        }
    }





?>
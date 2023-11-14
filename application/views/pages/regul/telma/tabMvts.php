<div id="tabMvts" class="container-fluid mt-1 tab-pane "><br>  
        <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div> 
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste MVTS Regularisé</b>
                </div>
                <div class="card-body">
                <table class="table table-bordered" id="tableMvts">
                <thead>
                <tr>
                    <th>Date</th>
                    <th>Heure</th>
                    <th>Transfer_id</th>
                    <th>Type</th>
                    <th>Channel</th>
                    <th>Etat</th>
                    <th>Montant</th>
                    <th>sender</th>
                    <th>receiver</th>
                    <th>Etat</th>
                    <th>Date_Regul</th>
                    <th>Action</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td></td>
                    <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalMvts"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
                </tr>
            </tbody>
            </table>
                </div>
            </div>

        </div>   
    </div>
    <?php $this->load->view("pages/regul/telma/modal/modalMvts"); ?>
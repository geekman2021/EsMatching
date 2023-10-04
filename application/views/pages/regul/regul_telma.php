<div class=" container mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabBoa">BOA</a>
        </li>
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabTelma">Telma</a>
        </li>
    </ul>
</div>


<div class="tab-content">
    <!-- BOA -->
    <div id="tabBoa" class="container mt-5 tab-pane "><br>  
        <div class="row">
        <div class="col-sm-6">
            <label for="dateDebut">Date Debut</label>
            <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6">
            <label for="dateDebut">Date Fin</label>
            <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div> 
        <table class="table table-bordered" id="tableBoa">
            <thead style="text-align: center ;">
                <tr>
                <th>Compte</th>
                <th>Date_Oper</th>
                <th>Date_Val </th>
                <th>Devise</th>
                <th>Montant</th>
                <th>Libelle</th>
                <th>Operation</th>
                <th>Expl</th>
                <th>ReferenceIgor</th>
                <th>Action</th>
                </tr>
            </thead>
            <tbody>
                <tr>
                <td></td>
                <td></td>
                <td> </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><a href="#" class="btn btn-info" data-toggle="modal" data-target="#myModalBoa" ></a></td>
                </tr>
            </tbody>
        </table>
    </div>

    
    <!-- Telma -->
    <div id="tabTelma" class="container mt-5 tab-pane ">
        <div class="row">
            <div class="col-sm-6">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
                </div>
                <div class="col-sm-6">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
                </div> 
            </div>
        <table id="tableTelma" class="table table-bordered">
            <thead>
            <tr> 
                <th>Transfer_id</th>
                <th>Date </th>
                <th>Heure</th>
                <th>Channel</th>
                <th>Type</th>
                <th>Etat</th>
                <th>Montant</th>
                <th>Envoyeur</th>
                <th>Receveur</th>
                <th>Action</th>
            </tr>
            </thead>
            <tbody>
            <?php //foreach($ci as $item)  { ?>
            <tr> 
                <td></td>
                <td></td>
                <td> </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><a href="#" data-toggle="modal" data-target="#myModalTelma<?php //echo $item->id; ?>">Afficher Infos</a></td>
            </tr>
            <?php //} ?>
            </tbody>
        </table>
    </div>
</div>


